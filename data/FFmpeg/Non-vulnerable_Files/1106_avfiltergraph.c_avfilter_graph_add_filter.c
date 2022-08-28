int avfilter_graph_add_filter(AVFilterGraph *graph, AVFilterContext *filter)
{
    AVFilterContext **filters = av_realloc(graph->filters,
                                           sizeof(*filters) * (graph->nb_filters + 1));
    if (!filters)
        return AVERROR(ENOMEM);
    graph->filters = filters;
    graph->filters[graph->nb_filters++] = filter;
#if FF_API_FOO_COUNT
FF_DISABLE_DEPRECATION_WARNINGS
    graph->filter_count_unused = graph->nb_filters;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    filter->graph = graph;
    return 0;
}
