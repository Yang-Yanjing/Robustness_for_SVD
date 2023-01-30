}
static sout_stream_id_sys_t * Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    sout_stream_id_sys_t *p_es = malloc( sizeof(sout_stream_id_sys_t) );
    p_es->fmt = *p_fmt;
    p_es->id = NULL;
    p_es->i_last = VLC_TS_INVALID;
    p_es->b_error = false;
    TAB_APPEND( p_sys->i_es_num, p_sys->pp_es, p_es );
    return p_es;
}
