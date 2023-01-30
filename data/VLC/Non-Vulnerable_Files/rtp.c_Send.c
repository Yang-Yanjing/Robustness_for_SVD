}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    assert( p_stream->p_sys->p_mux == NULL );
    (void)p_stream;
    while( p_buffer != NULL )
    {
        block_t *p_next = p_buffer->p_next;
        p_buffer->p_next = NULL;
        /* Send a Vorbis/Theora Packed Configuration packet (RFC 5215 §3.1)
         * as the first packet of the stream */
        if (id->b_first_packet)
        {
            id->b_first_packet = false;
            if (!strcmp(id->rtp_fmt.ptname, "vorbis") ||
                !strcmp(id->rtp_fmt.ptname, "theora"))
                rtp_packetize_xiph_config(id, id->rtp_fmt.fmtp,
                                          p_buffer->i_pts);
        }
        if( id->rtp_fmt.pf_packetize( id, p_buffer ) )
            break;
        p_buffer = p_next;
    }
    return VLC_SUCCESS;
}
