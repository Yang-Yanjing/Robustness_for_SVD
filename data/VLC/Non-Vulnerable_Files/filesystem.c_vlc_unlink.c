}
int vlc_unlink (const char *filename)
{
    wchar_t *wpath = widen_path (filename);
    if (wpath == NULL)
        return -1;
    int ret = _wunlink (wpath);
    free (wpath);
    return ret;
}
