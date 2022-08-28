const char *av_basename(const char *path)
{
    char *p = strrchr(path, '/');
#if HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');
    p = FFMAX3(p, q, d);
#endif
    if (!p)
        return path;
    return p + 1;
}
