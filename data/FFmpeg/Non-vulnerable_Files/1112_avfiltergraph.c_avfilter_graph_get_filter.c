AVFilterContext *avfilter_graph_get_filter(AVFilterGraph *graph, const char *name)
{
    int i;
    for (i = 0; i < graph->nb_filters; i++)
        if (graph->filters[i]->name && !strcmp(name, graph->filters[i]->name))
            return graph->filters[i];
    return NULL;
}
