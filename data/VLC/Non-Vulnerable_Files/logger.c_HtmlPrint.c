#endif
static void HtmlPrint( void *opaque, int type, const vlc_log_t *item,
                       const char *fmt, va_list ap )
{
    static const unsigned color[4] = {
        0xffffff, 0xff6666, 0xffff66, 0xaaaaaa,
    };
    intf_thread_t *p_intf = opaque;
    FILE *stream = p_intf->p_sys->p_file;
    if( IgnoreMessage( p_intf, type ) )
        return;
    int canc = vlc_savecancel();
    flockfile( stream );
    fprintf( stream, "%s%s: <span style=\"color: #%06x\">",
             item->psz_module, ppsz_type[type], color[type] );
    /* FIXME: encode special ASCII characters */
    vfprintf( stream, fmt, ap );
    fputs( "</span>\n", stream );
    funlockfile( stream );
    vlc_restorecancel( canc );
}
