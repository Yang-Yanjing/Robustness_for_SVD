#undef net_vaPrintf
ssize_t net_vaPrintf( vlc_object_t *p_this, int fd, const v_socket_t *p_vs,
                      const char *psz_fmt, va_list args )
{
    char    *psz;
    int      i_ret;
    int i_size = vasprintf( &psz, psz_fmt, args );
    if( i_size == -1 )
        return -1;
    i_ret = net_Write( p_this, fd, p_vs, psz, i_size ) < i_size
        ? -1 : i_size;
    free( psz );
    return i_ret;
}
