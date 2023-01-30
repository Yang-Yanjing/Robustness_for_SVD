}
static int DelIn( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    in_sout_stream_sys_t *p_sys = (in_sout_stream_sys_t *)p_stream->p_sys;
    if( id == p_sys->id_video ) p_sys->id_video = NULL;
    if( id == p_sys->id_audio ) p_sys->id_audio = NULL;
    int ret = p_stream->p_next->pf_del( p_stream->p_next, id->id );
    free( id );
    return ret;
}
