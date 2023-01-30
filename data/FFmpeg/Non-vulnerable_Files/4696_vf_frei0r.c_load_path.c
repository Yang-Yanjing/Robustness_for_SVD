static int load_path(AVFilterContext *ctx, void **handle_ptr, const char *prefix, const char *name)
{
    char *path = av_asprintf("%s%s%s", prefix, name, SLIBSUF);
    if (!path)
        return AVERROR(ENOMEM);
    av_log(ctx, AV_LOG_DEBUG, "Looking for frei0r effect in '%s'.\n", path);
    *handle_ptr = dlopen(path, RTLD_NOW|RTLD_LOCAL);
    av_free(path);
    return 0;
}
