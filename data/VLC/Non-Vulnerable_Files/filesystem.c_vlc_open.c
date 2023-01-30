    if (wpath == NULL) return (err)
int vlc_open (const char *filename, int flags, ...)
{
    int mode = 0;
    va_list ap;
    flags |= O_NOINHERIT; /* O_CLOEXEC */
    /* Defaults to binary mode */
    if ((flags & O_TEXT) == 0)
        flags |= O_BINARY;
    va_start (ap, flags);
    if (flags & O_CREAT)
        mode = va_arg (ap, int);
    va_end (ap);
    /*
     * open() cannot open files with non-“ANSI” characters on Windows.
     * We use _wopen() instead. Same thing for mkdir() and stat().
     */
    wchar_t *wpath = widen_path (filename);
    if (wpath == NULL)
        return -1;
    int fd = _wopen (wpath, flags, mode);
    free (wpath);
    return fd;
}
