};
static void AndroidPrint( void *opaque, int type, const vlc_log_t *item,
                       const char *fmt, va_list ap )
{
    (void)item;
    intf_thread_t *p_intf = opaque;
    if( IgnoreMessage( p_intf, type ) )
        return;
    int canc = vlc_savecancel();
    __android_log_vprint(prioritytype[type], "VLC", fmt, ap);
    vlc_restorecancel( canc );
}
