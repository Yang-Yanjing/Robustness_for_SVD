static void swap_channel_layouts(AVFilterGraph *graph)
{
    int i;
    for (i = 0; i < graph->nb_filters; i++)
        swap_channel_layouts_on_filter(graph->filters[i]);
}
