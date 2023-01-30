/** Add an ES to a non-RTP muxed stream */
static sout_stream_id_sys_t *MuxAdd( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_input_t      *p_input;
    sout_mux_t *p_mux = p_stream->p_sys->p_mux;
    assert( p_mux != NULL );
    p_input = sout_MuxAddStream( p_mux, p_fmt );
    if( p_input == NULL )
    {
        msg_Err( p_stream, "cannot add this stream to the muxer" );
        return NULL;
    }
    return (sout_stream_id_sys_t *)p_input;
}
