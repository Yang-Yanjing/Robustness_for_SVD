 *****************************************************************************/
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t *id;
    id = malloc( sizeof(*id) );
    if( !id )
        return NULL;
    es_format_Copy( &id->fmt, p_fmt );
    id->p_first = NULL;
    id->pp_last = &id->p_first;
    id->id = NULL;
    id->b_wait_key = true;
    id->b_wait_start = true;
    TAB_APPEND( p_sys->i_id, p_sys->id, id );
    return id;
}
