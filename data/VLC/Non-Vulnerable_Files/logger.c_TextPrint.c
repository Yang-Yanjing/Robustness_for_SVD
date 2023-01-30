#endif
static void TextPrint( void *opaque, int type, const vlc_log_t *item,
                       const char *fmt, va_list ap )
{
    intf_thread_t *p_intf = opaque;
    FILE *stream = p_intf->p_sys->p_file;
    if( IgnoreMessage( p_intf, type ) )
        return;
    int canc = vlc_savecancel();
    flockfile( stream );
    fprintf( stream, "%s%s: ", item->psz_module, ppsz_type[type] );
    vfprintf( stream, fmt, ap );
    putc_unlocked( '\n', stream );
    funlockfile( stream );
    vlc_restorecancel( canc );
}
