FADE(s32, int32_t)
static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AudioFadeContext *s  = ctx->priv;
    switch (outlink->format) {
    case AV_SAMPLE_FMT_DBL:  s->fade_samples = fade_samples_dbl;  break;
    case AV_SAMPLE_FMT_DBLP: s->fade_samples = fade_samples_dblp; break;
    case AV_SAMPLE_FMT_FLT:  s->fade_samples = fade_samples_flt;  break;
    case AV_SAMPLE_FMT_FLTP: s->fade_samples = fade_samples_fltp; break;
    case AV_SAMPLE_FMT_S16:  s->fade_samples = fade_samples_s16;  break;
    case AV_SAMPLE_FMT_S16P: s->fade_samples = fade_samples_s16p; break;
    case AV_SAMPLE_FMT_S32:  s->fade_samples = fade_samples_s32;  break;
    case AV_SAMPLE_FMT_S32P: s->fade_samples = fade_samples_s32p; break;
    }
    if (s->duration)
        s->nb_samples = av_rescale(s->duration, outlink->sample_rate, AV_TIME_BASE);
    if (s->start_time)
        s->start_sample = av_rescale(s->start_time, outlink->sample_rate, AV_TIME_BASE);
    return 0;
}
