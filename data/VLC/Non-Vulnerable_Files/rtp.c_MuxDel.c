/** Remove an ES from a non-RTP muxed stream */
static int MuxDel( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_mux_t *p_mux = p_stream->p_sys->p_mux;
    assert( p_mux != NULL );
    sout_MuxDeleteStream( p_mux, (sout_input_t *)id );
    return VLC_SUCCESS;
}
