#ifdef HAVE_SYSLOG_H
static void SyslogPrint( void *opaque, int type, const vlc_log_t *item,
                         const char *fmt, va_list ap )
{
    static const int i_prio[4] = { LOG_INFO, LOG_ERR, LOG_WARNING, LOG_DEBUG };
    intf_thread_t *p_intf = opaque;
    char *str;
    int i_priority = i_prio[type];
    if( IgnoreMessage( p_intf, type )
     || unlikely(vasprintf( &str, fmt, ap ) == -1) )
        return;
    int canc = vlc_savecancel();
    if( item->psz_header != NULL )
        syslog( i_priority, "[%s] %s%s: %s", item->psz_header,
                item->psz_module, ppsz_type[type], str );
    else
        syslog( i_priority, "%s%s: %s",
                item->psz_module, ppsz_type[type], str );
    vlc_restorecancel( canc );
    free( str );
}
