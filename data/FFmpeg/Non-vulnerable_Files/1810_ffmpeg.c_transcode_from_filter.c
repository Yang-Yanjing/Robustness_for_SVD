static int transcode_from_filter(FilterGraph *graph, InputStream **best_ist)
{
    int i, ret;
    int nb_requests, nb_requests_max = 0;
    InputFilter *ifilter;
    InputStream *ist;
    *best_ist = NULL;
    ret = avfilter_graph_request_oldest(graph->graph);
    if (ret >= 0)
        return reap_filters(0);
    if (ret == AVERROR_EOF) {
        ret = reap_filters(1);
        for (i = 0; i < graph->nb_outputs; i++)
            close_output_stream(graph->outputs[i]->ost);
        return ret;
    }
    if (ret != AVERROR(EAGAIN))
        return ret;
    for (i = 0; i < graph->nb_inputs; i++) {
        ifilter = graph->inputs[i];
        ist = ifilter->ist;
        if (input_files[ist->file_index]->eagain ||
            input_files[ist->file_index]->eof_reached)
            continue;
        nb_requests = av_buffersrc_get_nb_failed_requests(ifilter->filter);
        if (nb_requests > nb_requests_max) {
            nb_requests_max = nb_requests;
            *best_ist = ist;
        }
    }
    if (!*best_ist)
        for (i = 0; i < graph->nb_outputs; i++)
            graph->outputs[i]->ost->unavailable = 1;
    return 0;
}
