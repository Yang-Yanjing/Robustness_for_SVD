ECHO(s32, int32_t, INT32_MIN, INT32_MAX)
static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AudioEchoContext *s = ctx->priv;
    float volume = 1.0;
    int i;
    for (i = 0; i < s->nb_echoes; i++) {
        s->samples[i] = s->delay[i] * outlink->sample_rate / 1000.0;
        s->max_samples = FFMAX(s->max_samples, s->samples[i]);
        volume += s->decay[i];
    }
    if (s->max_samples <= 0) {
        av_log(ctx, AV_LOG_ERROR, "Nothing to echo - missing delay samples.\n");
        return AVERROR(EINVAL);
    }
    s->fade_out = s->max_samples;
    if (volume * s->in_gain * s->out_gain > 1.0)
        av_log(ctx, AV_LOG_WARNING,
               "out_gain %f can cause saturation of output\n", s->out_gain);
    switch (outlink->format) {
    case AV_SAMPLE_FMT_DBLP: s->echo_samples = echo_samples_dblp; break;
    case AV_SAMPLE_FMT_FLTP: s->echo_samples = echo_samples_fltp; break;
    case AV_SAMPLE_FMT_S16P: s->echo_samples = echo_samples_s16p; break;
    case AV_SAMPLE_FMT_S32P: s->echo_samples = echo_samples_s32p; break;
    }
    if (s->delayptrs)
        av_freep(&s->delayptrs[0]);
    av_freep(&s->delayptrs);
    return av_samples_alloc_array_and_samples(&s->delayptrs, NULL,
                                              outlink->channels,
                                              s->max_samples,
                                              outlink->format, 0);
}
