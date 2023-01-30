static void swap_samplerates(AVFilterGraph *graph)
{
    int i;
    for (i = 0; i < graph->nb_filters; i++)
        swap_samplerates_on_filter(graph->filters[i]);
}
