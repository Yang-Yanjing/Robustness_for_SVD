}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    if ( id == p_sys->id )
        p_sys->id = NULL;
    return p_stream->p_next->pf_del( p_stream->p_next, id );
}
