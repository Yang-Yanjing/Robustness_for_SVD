}
char *vlc_getcwd (void)
{
    wchar_t *wdir = _wgetcwd (NULL, 0);
    if (wdir == NULL)
        return NULL;
    char *dir = FromWide (wdir);
    free (wdir);
    return dir;
}
