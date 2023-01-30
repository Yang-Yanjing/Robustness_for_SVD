}
static void SetLanguage( sout_stream_t *p_stream, char *psz_language )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    if ( strncmp( p_sys->psz_language, psz_language, 3 ) )
        msg_Dbg( p_stream, "changing language to %s", psz_language );
    strncpy( p_sys->psz_language, (const char *)psz_language, 3 );
}
