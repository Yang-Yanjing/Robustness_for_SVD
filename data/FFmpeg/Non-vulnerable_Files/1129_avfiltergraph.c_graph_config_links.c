static int graph_config_links(AVFilterGraph *graph, AVClass *log_ctx)
{
    AVFilterContext *filt;
    int i, ret;
    for (i = 0; i < graph->nb_filters; i++) {
        filt = graph->filters[i];
        if (!filt->nb_outputs) {
            if ((ret = avfilter_config_links(filt)))
                return ret;
        }
    }
    return 0;
}
