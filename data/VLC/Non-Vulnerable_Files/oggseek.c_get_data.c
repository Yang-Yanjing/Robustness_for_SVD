/* read bytes from the ogg file to try to find a page start */
static int64_t get_data( demux_t *p_demux, int64_t i_bytes_to_read )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    char *buf;
    int64_t i_result;
    if ( p_sys->i_total_length > 0 )
    {
        if ( p_sys->i_input_position + i_bytes_to_read > p_sys->i_total_length )
        {
            i_bytes_to_read = p_sys->i_total_length - p_sys->i_input_position;
            if ( i_bytes_to_read <= 0 ) {
                return 0;
            }
        }
    }
    i_bytes_to_read = __MIN( i_bytes_to_read, INT_MAX );
    seek_byte ( p_demux, p_sys->i_input_position );
    buf = ogg_sync_buffer( &p_sys->oy, i_bytes_to_read );
    i_result = stream_Read( p_demux->s, buf, i_bytes_to_read );
    ogg_sync_wrote( &p_sys->oy, i_result );
    return i_result;
}
