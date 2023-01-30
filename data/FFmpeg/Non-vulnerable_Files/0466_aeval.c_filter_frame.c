static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    EvalContext *eval     = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    int nb_samples        = in->nb_samples;
    AVFrame *out;
    double t0;
    int i, j;
    
    out = ff_get_audio_buffer(outlink, nb_samples);
    if (!out)
        return AVERROR(ENOMEM);
    av_frame_copy_props(out, in);
    t0 = TS2T(in->pts, inlink->time_base);
    
    for (i = 0; i < nb_samples; i++, eval->n++) {
        eval->var_values[VAR_N] = eval->n;
        eval->var_values[VAR_T] = t0 + i * (double)1/inlink->sample_rate;
        for (j = 0; j < inlink->channels; j++)
            eval->channel_values[j] = *((double *) in->extended_data[j] + i);
        for (j = 0; j < outlink->channels; j++) {
            eval->var_values[VAR_CH] = j;
            *((double *) out->extended_data[j] + i) =
                av_expr_eval(eval->expr[j], eval->var_values, eval);
        }
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
