AVFilterContext *avfilter_graph_alloc_filter(AVFilterGraph *graph,
                                             const AVFilter *filter,
                                             const char *name)
{
    AVFilterContext **filters, *s;
    if (graph->thread_type && !graph->internal->thread_execute) {
        if (graph->execute) {
            graph->internal->thread_execute = graph->execute;
        } else {
            int ret = ff_graph_thread_init(graph);
            if (ret < 0) {
                av_log(graph, AV_LOG_ERROR, "Error initializing threading.\n");
                return NULL;
            }
        }
    }
    s = ff_filter_alloc(filter, name);
    if (!s)
        return NULL;
    filters = av_realloc(graph->filters, sizeof(*filters) * (graph->nb_filters + 1));
    if (!filters) {
        avfilter_free(s);
        return NULL;
    }
    graph->filters = filters;
    graph->filters[graph->nb_filters++] = s;
#if FF_API_FOO_COUNT
FF_DISABLE_DEPRECATION_WARNINGS
    graph->filter_count_unused = graph->nb_filters;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    s->graph = graph;
    return s;
}
