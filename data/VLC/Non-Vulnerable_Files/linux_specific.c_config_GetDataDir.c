}
char *config_GetDataDir (void)
{
    const char *path = getenv ("VLC_DATA_PATH");
    if (path != NULL)
        return strdup (path);
    char *libdir = config_GetLibDir ();
    if (libdir == NULL)
        return NULL; /* OOM */
    char *datadir = NULL;
    /* There are no clean ways to do this, are there?
     * Due to multilibs, we cannot simply append ../share/. */
    char *p = strstr (libdir, "/lib/");
    if (p != NULL)
    {
        char *p2;
        /* Deal with nested "lib" directories. Grmbl. */
        while ((p2 = strstr (p + 4, "/lib/")) != NULL)
            p = p2;
        *p = '\0';
        if (unlikely(asprintf (&datadir, "%s/share/"PACKAGE, libdir) == -1))
            datadir = NULL;
    }
    free (libdir);
    return (datadir != NULL) ? datadir : strdup (PKGDATADIR);
}
