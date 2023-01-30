}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *p_es )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    sout_stream_id_sys_t *id = p_es->id;
    TAB_REMOVE( p_sys->i_es_num, p_sys->pp_es, p_es );
    free( p_es );
    if ( id != NULL )
        return p_stream->p_next->pf_del( p_stream->p_next, id );
    else
        return VLC_SUCCESS;
}
