}
int vlc_stat (const char *filename, struct stat *buf)
{
    wchar_t *wpath = widen_path (filename);
    if (wpath == NULL)
        return -1;
    static_assert (sizeof (*buf) == sizeof (struct _stati64),
                   "Mismatched struct stat definition.");
    int ret = _wstati64 (wpath, buf);
    free (wpath);
    return ret;
}
