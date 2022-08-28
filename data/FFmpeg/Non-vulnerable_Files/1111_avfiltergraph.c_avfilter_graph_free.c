void avfilter_graph_free(AVFilterGraph **graph)
{
    if (!*graph)
        return;
    while ((*graph)->nb_filters)
        avfilter_free((*graph)->filters[0]);
    ff_graph_thread_free(*graph);
    av_freep(&(*graph)->sink_links);
    av_freep(&(*graph)->scale_sws_opts);
    av_freep(&(*graph)->aresample_swr_opts);
    av_freep(&(*graph)->resample_lavr_opts);
    av_freep(&(*graph)->filters);
    av_freep(&(*graph)->internal);
    av_freep(graph);
}
