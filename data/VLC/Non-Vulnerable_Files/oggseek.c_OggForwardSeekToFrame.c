}
static int64_t OggForwardSeekToFrame( demux_t *p_demux, int64_t i_pos1, int64_t i_pos2,
                logical_stream_t *p_stream, int64_t i_granulepos, bool b_fastseek )
{
    int64_t i_result;
    int64_t i_bytes_to_read;
    int64_t i_bytes_read;
    demux_sys_t *p_sys  = p_demux->p_sys;
    i_bytes_to_read = i_pos2 - i_pos1 + 1;
    seek_byte( p_demux, i_pos1 );
    if ( i_bytes_to_read > OGGSEEK_BYTES_TO_READ ) i_bytes_to_read = OGGSEEK_BYTES_TO_READ;
    OggDebug(
        msg_Dbg( p_demux, "Probing Fwd %"PRId64" %"PRId64" for granule %"PRId64,
        i_pos1, i_pos2, i_granulepos );
    );
    while ( 1 )
    {
        if ( p_sys->i_input_position >= i_pos2 )
            return SEGMENT_NOT_FOUND;
        /* read next chunk */
        if ( ! ( i_bytes_read = get_data( p_demux, i_bytes_to_read ) ) )
            return SEGMENT_NOT_FOUND;
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
    ogg_packet op;
    while( ogg_stream_packetout( &p_stream->os, &op ) > 0 ) {};
    packetStartCoordinates lastpacket = { -1, -1, -1 };
    while( 1 )
    {
        if ( p_sys->i_input_position >= i_pos2 )
        {
            /* reached the end of the search region and nothing was found */
            break;
        }
        p_sys->b_page_waiting = false;
        if ( ! ( i_result = oggseek_read_page( p_demux ) ) )
        {
            /* EOF */
            break;
        }
        // found a page
        if ( p_stream->os.serialno != ogg_page_serialno( &p_sys->current_page ) )
        {
            /* page is not for this stream */
            p_sys->i_input_position += i_result;
            continue;
        }
        if ( OggSeekToPacket( p_demux, p_stream, i_granulepos, &lastpacket, b_fastseek ) )
        {
            p_sys->i_input_position = lastpacket.i_pos;
            p_stream->i_skip_frames = 0;
            return p_sys->i_input_position;
        }
        /*  -> start of next page */
        p_sys->i_input_position += i_result;
    }
    return SEGMENT_NOT_FOUND;
}
