}
static int MuxSend( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                    block_t *p_buffer )
{
    sout_mux_t *p_mux = p_stream->p_sys->p_mux;
    assert( p_mux != NULL );
    return sout_MuxSendBuffer( p_mux, (sout_input_t *)id, p_buffer );
}
