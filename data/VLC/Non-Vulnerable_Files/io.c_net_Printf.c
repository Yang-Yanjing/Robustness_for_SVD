#undef net_Printf
ssize_t net_Printf( vlc_object_t *p_this, int fd, const v_socket_t *p_vs,
                    const char *psz_fmt, ... )
{
    int i_ret;
    va_list args;
    va_start( args, psz_fmt );
    i_ret = net_vaPrintf( p_this, fd, p_vs, psz_fmt, args );
    va_end( args );
    return i_ret;
}
