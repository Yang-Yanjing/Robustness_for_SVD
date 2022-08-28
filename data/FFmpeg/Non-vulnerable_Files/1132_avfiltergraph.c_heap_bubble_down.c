static void heap_bubble_down(AVFilterGraph *graph,
                             AVFilterLink *link, int index)
{
    AVFilterLink **links = graph->sink_links;
    av_assert0(index >= 0);
    while (1) {
        int child = 2 * index + 1;
        if (child >= graph->sink_links_count)
            break;
        if (child + 1 < graph->sink_links_count &&
            links[child + 1]->current_pts < links[child]->current_pts)
            child++;
        if (link->current_pts < links[child]->current_pts)
            break;
        links[index] = links[child];
        links[index]->age_index = index;
        index = child;
    }
    links[index] = link;
    link->age_index = index;
}
