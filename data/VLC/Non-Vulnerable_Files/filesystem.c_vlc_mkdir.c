}
int vlc_mkdir( const char *dirname, mode_t mode )
{
    wchar_t *wpath = widen_path (dirname);
    if (wpath == NULL)
        return -1;
    int ret = _wmkdir (wpath);
    free (wpath);
    (void) mode;
    return ret;
}
