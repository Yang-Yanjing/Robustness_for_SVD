void ff_filter_graph_remove_filter(AVFilterGraph *graph, AVFilterContext *filter)
{
    int i;
    for (i = 0; i < graph->nb_filters; i++) {
        if (graph->filters[i] == filter) {
            FFSWAP(AVFilterContext*, graph->filters[i],
                   graph->filters[graph->nb_filters - 1]);
            graph->nb_filters--;
            return;
        }
    }
}
