static int graph_check_validity(AVFilterGraph *graph, AVClass *log_ctx)
{
    AVFilterContext *filt;
    int i, j;
    for (i = 0; i < graph->nb_filters; i++) {
        const AVFilterPad *pad;
        filt = graph->filters[i];
        for (j = 0; j < filt->nb_inputs; j++) {
            if (!filt->inputs[j] || !filt->inputs[j]->src) {
                pad = &filt->input_pads[j];
                av_log(log_ctx, AV_LOG_ERROR,
                       "Input pad \"%s\" with type %s of the filter instance \"%s\" of %s not connected to any source\n",
                       pad->name, av_get_media_type_string(pad->type), filt->name, filt->filter->name);
                return AVERROR(EINVAL);
            }
        }
        for (j = 0; j < filt->nb_outputs; j++) {
            if (!filt->outputs[j] || !filt->outputs[j]->dst) {
                pad = &filt->output_pads[j];
                av_log(log_ctx, AV_LOG_ERROR,
                       "Output pad \"%s\" with type %s of the filter instance \"%s\" of %s not connected to any destination\n",
                       pad->name, av_get_media_type_string(pad->type), filt->name, filt->filter->name);
                return AVERROR(EINVAL);
            }
        }
    }
    return 0;
}
