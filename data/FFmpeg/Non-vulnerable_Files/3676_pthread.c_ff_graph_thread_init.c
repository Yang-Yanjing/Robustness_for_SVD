int ff_graph_thread_init(AVFilterGraph *graph)
{
    int ret;
#if HAVE_W32THREADS
    w32thread_init();
#endif
    if (graph->nb_threads == 1) {
        graph->thread_type = 0;
        return 0;
    }
    graph->internal->thread = av_mallocz(sizeof(ThreadContext));
    if (!graph->internal->thread)
        return AVERROR(ENOMEM);
    ret = thread_init_internal(graph->internal->thread, graph->nb_threads);
    if (ret <= 1) {
        av_freep(&graph->internal->thread);
        graph->thread_type = 0;
        graph->nb_threads  = 1;
        return (ret < 0) ? ret : 0;
    }
    graph->nb_threads = ret;
    graph->internal->thread_execute = thread_execute;
    return 0;
}
