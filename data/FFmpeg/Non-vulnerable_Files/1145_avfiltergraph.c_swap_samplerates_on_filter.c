static void swap_samplerates_on_filter(AVFilterContext *filter)
{
    AVFilterLink *link = NULL;
    int sample_rate;
    int i, j;
    for (i = 0; i < filter->nb_inputs; i++) {
        link = filter->inputs[i];
        if (link->type == AVMEDIA_TYPE_AUDIO &&
            link->out_samplerates->nb_formats== 1)
            break;
    }
    if (i == filter->nb_inputs)
        return;
    sample_rate = link->out_samplerates->formats[0];
    for (i = 0; i < filter->nb_outputs; i++) {
        AVFilterLink *outlink = filter->outputs[i];
        int best_idx, best_diff = INT_MAX;
        if (outlink->type != AVMEDIA_TYPE_AUDIO ||
            outlink->in_samplerates->nb_formats < 2)
            continue;
        for (j = 0; j < outlink->in_samplerates->nb_formats; j++) {
            int diff = abs(sample_rate - outlink->in_samplerates->formats[j]);
            if (diff < best_diff) {
                best_diff = diff;
                best_idx  = j;
            }
        }
        FFSWAP(int, outlink->in_samplerates->formats[0],
               outlink->in_samplerates->formats[best_idx]);
    }
}
