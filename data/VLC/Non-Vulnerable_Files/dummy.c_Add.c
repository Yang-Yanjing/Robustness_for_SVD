}
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    VLC_UNUSED(p_stream); VLC_UNUSED(p_fmt);
    return malloc( 1 );
}
