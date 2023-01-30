static void *try_load(const char *dir, const char *soname)
{
    char *path = av_asprintf("%s/%s.so", dir, soname);
    void *ret = NULL;
    if (path) {
        ret = dlopen(path, RTLD_LOCAL|RTLD_NOW);
        av_free(path);
    }
    return ret;
}
