static void heap_bubble_up(AVFilterGraph *graph,
                           AVFilterLink *link, int index)
{
    AVFilterLink **links = graph->sink_links;
    av_assert0(index >= 0);
    while (index) {
        int parent = (index - 1) >> 1;
        if (links[parent]->current_pts >= link->current_pts)
            break;
        links[index] = links[parent];
        links[index]->age_index = index;
        index = parent;
    }
    links[index] = link;
    link->age_index = index;
}
