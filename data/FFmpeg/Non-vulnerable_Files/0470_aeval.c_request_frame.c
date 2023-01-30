static int request_frame(AVFilterLink *outlink)
{
    EvalContext *eval = outlink->src->priv;
    AVFrame *samplesref;
    int i, j;
    int64_t t = av_rescale(eval->n, AV_TIME_BASE, eval->sample_rate);
    if (eval->duration >= 0 && t >= eval->duration)
        return AVERROR_EOF;
    samplesref = ff_get_audio_buffer(outlink, eval->nb_samples);
    if (!samplesref)
        return AVERROR(ENOMEM);
    
    for (i = 0; i < eval->nb_samples; i++, eval->n++) {
        eval->var_values[VAR_N] = eval->n;
        eval->var_values[VAR_T] = eval->var_values[VAR_N] * (double)1/eval->sample_rate;
        for (j = 0; j < eval->nb_channels; j++) {
            *((double *) samplesref->extended_data[j] + i) =
                av_expr_eval(eval->expr[j], eval->var_values, NULL);
        }
    }
    samplesref->pts = eval->pts;
    samplesref->sample_rate = eval->sample_rate;
    eval->pts += eval->nb_samples;
    return ff_filter_frame(outlink, samplesref);
}
