int avfilter_open(AVFilterContext **filter_ctx, AVFilter *filter, const char *inst_name)
{
    *filter_ctx = ff_filter_alloc(filter, inst_name);
    return *filter_ctx ? 0 : AVERROR(ENOMEM);
}
