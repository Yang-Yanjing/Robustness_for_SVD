}
int vlc_lstat (const char *filename, struct stat *buf)
{
    return vlc_stat (filename, buf);
}
