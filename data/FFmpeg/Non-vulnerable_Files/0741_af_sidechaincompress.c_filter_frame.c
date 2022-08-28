static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    AVFilterContext *ctx = link->dst;
    SidechainCompressContext *s = ctx->priv;
    AVFilterLink *sclink = ctx->inputs[1];
    AVFilterLink *outlink = ctx->outputs[0];
    const double makeup = s->makeup;
    const double *scsrc;
    double *sample;
    int nb_samples;
    int ret, i, c;
    for (i = 0; i < 2; i++)
        if (link == ctx->inputs[i])
            break;
    av_assert0(i < 2 && !s->input_frame[i]);
    s->input_frame[i] = frame;
    if (!s->input_frame[0] || !s->input_frame[1])
        return 0;
    nb_samples = FFMIN(s->input_frame[0]->nb_samples,
                       s->input_frame[1]->nb_samples);
    sample = (double *)s->input_frame[0]->data[0];
    scsrc = (const double *)s->input_frame[1]->data[0];
    for (i = 0; i < nb_samples; i++) {
        double abs_sample, gain = 1.0;
        abs_sample = FFABS(scsrc[0]);
        if (s->link == 1) {
            for (c = 1; c < sclink->channels; c++)
                abs_sample = FFMAX(FFABS(scsrc[c]), abs_sample);
        } else {
            for (c = 1; c < sclink->channels; c++)
                abs_sample += FFABS(scsrc[c]);
            abs_sample /= sclink->channels;
        }
        if (s->detection)
            abs_sample *= abs_sample;
        s->lin_slope += (abs_sample - s->lin_slope) * (abs_sample > s->lin_slope ? s->attack_coeff : s->release_coeff);
        if (s->lin_slope > 0.0 && s->lin_slope > s->lin_knee_start)
            gain = output_gain(s->lin_slope, s->ratio, s->thres, s->knee,
                               s->knee_start, s->knee_stop,
                               s->compressed_knee_stop, s->detection);
        for (c = 0; c < outlink->channels; c++)
            sample[c] *= gain * makeup;
        sample += outlink->channels;
        scsrc += sclink->channels;
    }
    ret = ff_filter_frame(outlink, s->input_frame[0]);
    s->input_frame[0] = NULL;
    av_frame_free(&s->input_frame[1]);
    return ret;
}
