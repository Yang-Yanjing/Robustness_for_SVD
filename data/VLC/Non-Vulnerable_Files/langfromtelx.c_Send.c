 *****************************************************************************/
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    if ( id == p_sys->p_telx )
        HandleTelx( p_stream, p_buffer );
    return p_stream->p_next->pf_send( p_stream->p_next, id, p_buffer );
}
