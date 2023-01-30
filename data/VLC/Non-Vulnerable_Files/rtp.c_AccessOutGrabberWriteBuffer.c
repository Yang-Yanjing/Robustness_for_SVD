}
static ssize_t AccessOutGrabberWriteBuffer( sout_stream_t *p_stream,
                                            const block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t *id = p_sys->es[0];
    int64_t  i_dts = p_buffer->i_dts;
    uint8_t         *p_data = p_buffer->p_buffer;
    size_t          i_data  = p_buffer->i_buffer;
    size_t          i_max   = id->i_mtu - 12;
    size_t i_packet = ( p_buffer->i_buffer + i_max - 1 ) / i_max;
    while( i_data > 0 )
    {
        size_t i_size;
        /* output complete packet */
        if( p_sys->packet &&
            p_sys->packet->i_buffer + i_data > i_max )
        {
            rtp_packetize_send( id, p_sys->packet );
            p_sys->packet = NULL;
        }
        if( p_sys->packet == NULL )
        {
            /* allocate a new packet */
            p_sys->packet = block_Alloc( id->i_mtu );
            rtp_packetize_common( id, p_sys->packet, 1, i_dts );
            p_sys->packet->i_dts = i_dts;
            p_sys->packet->i_length = p_buffer->i_length / i_packet;
            i_dts += p_sys->packet->i_length;
        }
        i_size = __MIN( i_data,
                        (unsigned)(id->i_mtu - p_sys->packet->i_buffer) );
        memcpy( &p_sys->packet->p_buffer[p_sys->packet->i_buffer],
                p_data, i_size );
        p_sys->packet->i_buffer += i_size;
        p_data += i_size;
        i_data -= i_size;
    }
    return VLC_SUCCESS;
}
