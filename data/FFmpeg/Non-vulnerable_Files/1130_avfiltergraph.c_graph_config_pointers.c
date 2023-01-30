static int graph_config_pointers(AVFilterGraph *graph,
                                             AVClass *log_ctx)
{
    unsigned i, j;
    int sink_links_count = 0, n = 0;
    AVFilterContext *f;
    AVFilterLink **sinks;
    for (i = 0; i < graph->nb_filters; i++) {
        f = graph->filters[i];
        for (j = 0; j < f->nb_inputs; j++) {
            f->inputs[j]->graph     = graph;
            f->inputs[j]->age_index = -1;
        }
        for (j = 0; j < f->nb_outputs; j++) {
            f->outputs[j]->graph    = graph;
            f->outputs[j]->age_index= -1;
        }
        if (!f->nb_outputs) {
            if (f->nb_inputs > INT_MAX - sink_links_count)
                return AVERROR(EINVAL);
            sink_links_count += f->nb_inputs;
        }
    }
    sinks = av_calloc(sink_links_count, sizeof(*sinks));
    if (!sinks)
        return AVERROR(ENOMEM);
    for (i = 0; i < graph->nb_filters; i++) {
        f = graph->filters[i];
        if (!f->nb_outputs) {
            for (j = 0; j < f->nb_inputs; j++) {
                sinks[n] = f->inputs[j];
                f->inputs[j]->age_index = n++;
            }
        }
    }
    av_assert0(n == sink_links_count);
    graph->sink_links       = sinks;
    graph->sink_links_count = sink_links_count;
    return 0;
}
