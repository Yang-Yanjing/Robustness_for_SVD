}
static char *relative_URI(const char *psz_url, const char *psz_path)
{
    char *ret = NULL;
    const char *fmt;
    assert(psz_url != NULL && psz_path != NULL);
    //If the path is actually an absolute URL, don't do anything.
    if (strncmp(psz_path, "http", 4) == 0)
        return NULL;
    size_t len = strlen(psz_path);
    char *new_url = strdup(psz_url);
    if (unlikely(!new_url))
        return NULL;
    if( psz_path[0] == '/' ) //Relative URL with absolute path
    {
        //Try to find separator for name and path, try to skip
        //access and first ://
        char *slash = strchr(&new_url[8], '/');
        if (unlikely(slash == NULL))
            goto end;
        *slash = '\0';
        fmt = "%s%s";
    } else {
        int levels = 0;
        while(len >= 3 && !strncmp(psz_path, "../", 3)) {
            psz_path += 3;
            len -= 3;
            levels++;
        }
        do {
            char *slash = strrchr(new_url, '/');
            if (unlikely(slash == NULL))
                goto end;
            *slash = '\0';
        } while (levels--);
	fmt = "%s/%s";
    }
    if (asprintf(&ret, fmt, new_url, psz_path) < 0)
        ret = NULL;
end:
    free(new_url);
    return ret;
}
