static int insert_trim(int64_t start_time, int64_t duration,
                       AVFilterContext **last_filter, int *pad_idx,
                       const char *filter_name)
{
    AVFilterGraph *graph = (*last_filter)->graph;
    AVFilterContext *ctx;
    const AVFilter *trim;
    enum AVMediaType type = avfilter_pad_get_type((*last_filter)->output_pads, *pad_idx);
    const char *name = (type == AVMEDIA_TYPE_VIDEO) ? "trim" : "atrim";
    int ret = 0;
    if (duration == INT64_MAX && start_time == AV_NOPTS_VALUE)
        return 0;
    trim = avfilter_get_by_name(name);
    if (!trim) {
        av_log(NULL, AV_LOG_ERROR, "%s filter not present, cannot limit "
               "recording time.\n", name);
        return AVERROR_FILTER_NOT_FOUND;
    }
    ctx = avfilter_graph_alloc_filter(graph, trim, filter_name);
    if (!ctx)
        return AVERROR(ENOMEM);
    if (duration != INT64_MAX) {
        ret = av_opt_set_int(ctx, "durationi", duration,
                                AV_OPT_SEARCH_CHILDREN);
    }
    if (ret >= 0 && start_time != AV_NOPTS_VALUE) {
        ret = av_opt_set_int(ctx, "starti", start_time,
                                AV_OPT_SEARCH_CHILDREN);
    }
    if (ret < 0) {
        av_log(ctx, AV_LOG_ERROR, "Error configuring the %s filter", name);
        return ret;
    }
    ret = avfilter_init_str(ctx, NULL);
    if (ret < 0)
        return ret;
    ret = avfilter_link(*last_filter, *pad_idx, ctx, 0);
    if (ret < 0)
        return ret;
    *last_filter = ctx;
    *pad_idx     = 0;
    return 0;
}
