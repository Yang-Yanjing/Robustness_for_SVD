void ff_update_link_current_pts(AVFilterLink *link, int64_t pts)
{
    if (pts == AV_NOPTS_VALUE)
        return;
    link->current_pts = av_rescale_q(pts, link->time_base, AV_TIME_BASE_Q);
    
    if (link->graph && link->age_index >= 0)
        ff_avfilter_graph_update_heap(link->graph, link);
}
