}
void Oggseek_ProbeEnd( demux_t *p_demux )
{
    /* Temporary state */
    ogg_stream_state os;
    ogg_sync_state oy;
    ogg_page page;
    demux_sys_t *p_sys = p_demux->p_sys;
    int64_t i_pos, i_startpos, i_result, i_granule, i_lowerbound;
    int64_t i_length = 0;
    int64_t i_backup_pos = stream_Tell( p_demux->s );
    int64_t i_upperbound = stream_Size( p_demux->s );
    unsigned int i_backoffset = OGGSEEK_BYTES_TO_READ;
    assert( OGGSEEK_BYTES_TO_READ < UINT_MAX );
    const char *buffer;
    ogg_stream_init( &os, -1 );
    ogg_sync_init( &oy );
    /* Try to lookup last granule from each logical stream */
    i_lowerbound = stream_Size( p_demux->s ) - p_sys->i_streams * MAX_PAGE_SIZE * 2;
    i_lowerbound = __MAX( 0, i_lowerbound );
    i_pos = i_startpos = __MAX( i_lowerbound, i_upperbound - i_backoffset );
    if ( stream_Seek( p_demux->s, i_pos ) )
    {
        ogg_sync_clear( &oy );
        ogg_stream_clear( &os );
        return;
    }
    while( i_pos >= i_lowerbound )
    {
        while( i_pos < i_upperbound )
        {
            if ( oy.unsynced )
                i_result = ogg_sync_pageseek( &oy, &page );
            buffer = ogg_sync_buffer( &oy, OGGSEEK_BYTES_TO_READ );
            if ( buffer == NULL ) goto clean;
            i_result = stream_Read( p_demux->s, (void*) buffer, OGGSEEK_BYTES_TO_READ );
            if ( i_result < 1 ) goto clean;
            i_pos += i_result;
            ogg_sync_wrote( &oy, i_result );
            while( ogg_sync_pageout( &oy, &page ) == 1 )
            {
                i_granule = ogg_page_granulepos( &page );
                if ( i_granule == -1 ) continue;
                for ( int i=0; i< p_sys->i_streams; i++ )
                {
                    if ( p_sys->pp_stream[i]->i_serial_no != ogg_page_serialno( &page ) )
                        continue;
                    i_length = Oggseek_GranuleToAbsTimestamp( p_sys->pp_stream[i], i_granule, false );
                    p_sys->i_length = __MAX( p_sys->i_length, i_length / 1000000 );
                    break;
                }
            }
            if ( i_length > 0 ) break;
        }
        /* We found at least a page with valid granule */
        if ( i_length > 0 ) break;
        /* Otherwise increase read size, starting earlier */
        if ( i_backoffset <= ( UINT_MAX >> 1 ) )
        {
            i_backoffset <<= 1;
            i_startpos = i_upperbound - i_backoffset;
        }
        else
        {
            i_startpos -= i_backoffset;
        }
        i_pos = i_startpos;
        if ( stream_Seek( p_demux->s, i_pos ) )
            break;
    }
clean:
    stream_Seek( p_demux->s, i_backup_pos );
    ogg_sync_clear( &oy );
    ogg_stream_clear( &os );
}
