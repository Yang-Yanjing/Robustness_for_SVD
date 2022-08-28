static int graph_insert_fifos(AVFilterGraph *graph, AVClass *log_ctx)
{
    AVFilterContext *f;
    int i, j, ret;
    int fifo_count = 0;
    for (i = 0; i < graph->nb_filters; i++) {
        f = graph->filters[i];
        for (j = 0; j < f->nb_inputs; j++) {
            AVFilterLink *link = f->inputs[j];
            AVFilterContext *fifo_ctx;
            AVFilter *fifo;
            char name[32];
            if (!link->dstpad->needs_fifo)
                continue;
            fifo = f->inputs[j]->type == AVMEDIA_TYPE_VIDEO ?
                   avfilter_get_by_name("fifo") :
                   avfilter_get_by_name("afifo");
            snprintf(name, sizeof(name), "auto-inserted fifo %d", fifo_count++);
            ret = avfilter_graph_create_filter(&fifo_ctx, fifo, name, NULL,
                                               NULL, graph);
            if (ret < 0)
                return ret;
            ret = avfilter_insert_filter(link, fifo_ctx, 0, 0);
            if (ret < 0)
                return ret;
        }
    }
    return 0;
}
