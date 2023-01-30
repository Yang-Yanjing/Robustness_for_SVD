#define MAXLEN 256
static int ReadULSysfs( const char *psz_fmt, unsigned int i_link )
{
    char psz_file[MAXLEN], psz_data[MAXLEN];
    char *psz_tmp;
    int i_fd;
    ssize_t i_ret;
    unsigned int i_data;
    snprintf( psz_file, sizeof(psz_file) - 1, psz_fmt, i_link );
    if ( (i_fd = vlc_open( psz_file, O_RDONLY )) < 0 )
        return i_fd;
    i_ret = read( i_fd, psz_data, sizeof(psz_data) );
    close( i_fd );
    if ( i_ret < 0 )
        return i_ret;
    i_data = strtoul( psz_data, &psz_tmp, 0 );
    if ( *psz_tmp != '\n' )
        return -1;
    return i_data;
}
