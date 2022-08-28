void ff_avfilter_graph_update_heap(AVFilterGraph *graph, AVFilterLink *link)
{
    heap_bubble_up  (graph, link, link->age_index);
    heap_bubble_down(graph, link, link->age_index);
}
