const char *av_dirname(char *path)
{
    char *p = strrchr(path, '/');
#if HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');
    d = d ? d + 1 : d;
    p = FFMAX3(p, q, d);
#endif
    if (!p)
        return ".";
    *p = '\0';
    return path;
}
