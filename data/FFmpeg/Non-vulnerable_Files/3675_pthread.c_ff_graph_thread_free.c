void ff_graph_thread_free(AVFilterGraph *graph)
{
    if (graph->internal->thread)
        slice_thread_uninit(graph->internal->thread);
    av_freep(&graph->internal->thread);
}
