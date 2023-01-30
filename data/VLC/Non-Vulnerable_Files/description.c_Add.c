}
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t *id;
    es_format_t *p_fmt_copy;
    msg_Dbg( p_stream, "Adding a stream" );
    p_fmt_copy = malloc(sizeof(es_format_t));
    es_format_Copy( p_fmt_copy, p_fmt );
    TAB_APPEND( p_sys->data->i_es, p_sys->data->es, p_fmt_copy );
    if( p_sys->i_stream_start <= 0 )
        p_sys->i_stream_start = mdate();
    id = malloc( sizeof( sout_stream_id_sys_t ) );
    return id;
}
