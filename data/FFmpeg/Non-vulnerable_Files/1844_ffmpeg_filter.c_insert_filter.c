static int insert_filter(AVFilterContext **last_filter, int *pad_idx,
                         const char *filter_name, const char *args)
{
    AVFilterGraph *graph = (*last_filter)->graph;
    AVFilterContext *ctx;
    int ret;
    ret = avfilter_graph_create_filter(&ctx,
                                       avfilter_get_by_name(filter_name),
                                       filter_name, args, NULL, graph);
    if (ret < 0)
        return ret;
    ret = avfilter_link(*last_filter, *pad_idx, ctx, 0);
    if (ret < 0)
        return ret;
    *last_filter = ctx;
    *pad_idx     = 0;
    return 0;
}
