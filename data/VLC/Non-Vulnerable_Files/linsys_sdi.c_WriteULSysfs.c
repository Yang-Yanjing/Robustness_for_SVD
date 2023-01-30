}
static ssize_t WriteULSysfs( const char *psz_fmt, unsigned int i_link,
                             unsigned int i_buf )
{
    char psz_file[MAXLEN], psz_data[MAXLEN];
    int i_fd;
    ssize_t i_ret;
    snprintf( psz_file, sizeof(psz_file) -1, psz_fmt, i_link );
    snprintf( psz_data, sizeof(psz_data) -1, "%u\n", i_buf );
    if ( (i_fd = vlc_open( psz_file, O_WRONLY )) < 0 )
        return i_fd;
    i_ret = write( i_fd, psz_data, strlen(psz_data) + 1 );
    close( i_fd );
    return i_ret;
}
