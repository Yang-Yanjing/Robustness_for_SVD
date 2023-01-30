#endif
static wchar_t *widen_path (const char *path)
{
    wchar_t *wpath;
    errno = 0;
    wpath = ToWide (path);
    if (wpath == NULL)
    {
        if (errno == 0)
            errno = ENOENT;
        return NULL;
    }
    return wpath;
}
