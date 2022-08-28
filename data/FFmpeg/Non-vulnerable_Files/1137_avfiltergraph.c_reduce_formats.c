static void reduce_formats(AVFilterGraph *graph)
{
    int i, reduced;
    do {
        reduced = 0;
        for (i = 0; i < graph->nb_filters; i++)
            reduced |= reduce_formats_on_filter(graph->filters[i]);
    } while (reduced);
}
