/* XXX: evil code duplication from access.c */
static char *get_path(const char *location)
{
    char *url, *path;
    /* Prepending "file://" is a bit hackish. But then again, we do not want
     * to hard-code the list of schemes that use file paths in make_path().
     */
    if (asprintf(&url, "file://%s", location) == -1)
        return NULL;
    path = make_path (url);
    free (url);
    return path;
}
