int avfilter_graph_request_oldest(AVFilterGraph *graph)
{
    while (graph->sink_links_count) {
        AVFilterLink *oldest = graph->sink_links[0];
        int r = ff_request_frame(oldest);
        if (r != AVERROR_EOF)
            return r;
        av_log(oldest->dst, AV_LOG_DEBUG, "EOF on sink link %s:%s.\n",
               oldest->dst ? oldest->dst->name : "unknown",
               oldest->dstpad ? oldest->dstpad->name : "unknown");
        
        if (oldest->age_index < --graph->sink_links_count)
            heap_bubble_down(graph, graph->sink_links[graph->sink_links_count],
                             oldest->age_index);
        oldest->age_index = -1;
    }
    return AVERROR_EOF;
}
