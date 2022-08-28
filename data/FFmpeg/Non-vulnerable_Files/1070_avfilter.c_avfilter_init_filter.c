int avfilter_init_filter(AVFilterContext *filter, const char *args, void *opaque)
{
    return avfilter_init_str(filter, args);
}
