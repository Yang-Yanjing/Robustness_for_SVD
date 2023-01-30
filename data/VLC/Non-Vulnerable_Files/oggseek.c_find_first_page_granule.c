}
static int64_t find_first_page_granule( demux_t *p_demux,
                                int64_t i_pos1, int64_t i_pos2,
                                logical_stream_t *p_stream,
                                int64_t *i_granulepos )
{
    int64_t i_result;
    *i_granulepos = -1;
    int64_t i_bytes_to_read = i_pos2 - i_pos1 + 1;
    int64_t i_bytes_read;
    int64_t i_pages_checked = 0;
    int64_t i_packets_checked;
    demux_sys_t *p_sys  = p_demux->p_sys;
    ogg_packet op;
    seek_byte( p_demux, i_pos1 );
    if ( i_pos1 == p_stream->i_data_start )
        return p_sys->i_input_position;
    if ( i_bytes_to_read > OGGSEEK_BYTES_TO_READ ) i_bytes_to_read = OGGSEEK_BYTES_TO_READ;
    while ( 1 )
    {
        if ( p_sys->i_input_position >= i_pos2 )
        {
            /* we reached the end and found no pages */
            return -1;
        }
        /* read next chunk */
        if ( ! ( i_bytes_read = get_data( p_demux, i_bytes_to_read ) ) )
        {
            /* EOF */
            return -1;
        }
        i_bytes_to_read = OGGSEEK_BYTES_TO_READ;
        i_result = ogg_sync_pageseek( &p_sys->oy, &p_sys->current_page );
        if ( i_result < 0 )
        {
            /* found a page, sync to page start */
            p_sys->i_input_position -= i_result;
            i_pos1 = p_sys->i_input_position;
            continue;
        }
        if ( i_result > 0 || ( i_result == 0 && p_sys->oy.fill > 3 &&
                               ! strncmp( (char *)p_sys->oy.data, "OggS" , 4 ) ) )
        {
            i_pos1 = p_sys->i_input_position;
            break;
        }
        p_sys->i_input_position += i_bytes_read;
    };
    seek_byte( p_demux, p_sys->i_input_position );
    ogg_stream_reset( &p_stream->os );
    while( 1 )
    {
        if ( p_sys->i_input_position >= i_pos2 )
        {
            /* reached the end of the search region and nothing was found */
            return p_sys->i_input_position;
        }
        p_sys->b_page_waiting = false;
        if ( ! ( i_result = oggseek_read_page( p_demux ) ) )
        {
            /* EOF */
            return p_sys->i_input_position;
        }
        if ( ogg_page_granulepos( &p_sys->current_page ) <= 0 )
        {
            p_sys->i_input_position += i_result;
            i_bytes_to_read += OGGSEEK_BYTES_TO_READ;
            continue;
        }
        // found a page
        if ( ogg_stream_pagein( &p_stream->os, &p_sys->current_page ) != 0 )
        {
            /* page is not for this stream or incomplete */
            p_sys->i_input_position += i_result;
            if ( ! i_pages_checked ) i_pos1 = p_sys->i_input_position;
            continue;
        }
        i_pages_checked++;
        i_packets_checked = 0;
        while ( ogg_stream_packetout( &p_stream->os, &op ) > 0 )
        {
            i_packets_checked++;
        }
        if ( i_packets_checked )
        {
            *i_granulepos = ogg_page_granulepos( &p_sys->current_page );
            return i_pos1;
        }
        /*  -> start of next page */
        p_sys->i_input_position += i_result;
    }
}
