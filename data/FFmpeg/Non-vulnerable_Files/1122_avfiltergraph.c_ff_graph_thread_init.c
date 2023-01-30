int ff_graph_thread_init(AVFilterGraph *graph)
{
    graph->thread_type = 0;
    graph->nb_threads  = 1;
    return 0;
}
