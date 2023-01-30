}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    if ( id == p_sys->p_id ) p_sys->p_id = NULL;
    if ( id == p_sys->p_telx ) p_sys->p_telx = NULL;
    return p_stream->p_next->pf_del( p_stream->p_next, id );
}
