static void swap_sample_fmts(AVFilterGraph *graph)
{
    int i;
    for (i = 0; i < graph->nb_filters; i++)
        swap_sample_fmts_on_filter(graph->filters[i]);
}
