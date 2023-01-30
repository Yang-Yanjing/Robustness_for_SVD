VLC_FORMAT(2, 3)
static void msg_rc( intf_thread_t *p_intf, const char *psz_fmt, ... )
{
    va_list args;
    char fmt_eol[strlen (psz_fmt) + 3];
    snprintf (fmt_eol, sizeof (fmt_eol), "%s\r\n", psz_fmt);
    va_start( args, psz_fmt );
    if( p_intf->p_sys->i_socket == -1 )
        utf8_vfprintf( stdout, fmt_eol, args );
    else
        net_vaPrintf( p_intf, p_intf->p_sys->i_socket, NULL, fmt_eol, args );
    va_end( args );
}
