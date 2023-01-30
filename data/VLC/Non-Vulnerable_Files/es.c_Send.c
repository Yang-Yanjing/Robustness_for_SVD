}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    VLC_UNUSED(p_stream);
    return sout_MuxSendBuffer( id->p_mux, id->p_input, p_buffer );
}
