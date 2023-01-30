DELAY(dbl, double,  0)
static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    AudioDelayContext *s = ctx->priv;
    char *p, *arg, *saveptr = NULL;
    int i;
    s->chandelay = av_calloc(inlink->channels, sizeof(*s->chandelay));
    if (!s->chandelay)
        return AVERROR(ENOMEM);
    s->nb_delays = inlink->channels;
    s->block_align = av_get_bytes_per_sample(inlink->format);
    p = s->delays;
    for (i = 0; i < s->nb_delays; i++) {
        ChanDelay *d = &s->chandelay[i];
        float delay;
        if (!(arg = av_strtok(p, "|", &saveptr)))
            break;
        p = NULL;
        sscanf(arg, "%f", &delay);
        d->delay = delay * inlink->sample_rate / 1000.0;
        if (d->delay < 0) {
            av_log(ctx, AV_LOG_ERROR, "Delay must be non negative number.\n");
            return AVERROR(EINVAL);
        }
    }
    for (i = 0; i < s->nb_delays; i++) {
        ChanDelay *d = &s->chandelay[i];
        if (!d->delay)
            continue;
        d->samples = av_malloc_array(d->delay, s->block_align);
        if (!d->samples)
            return AVERROR(ENOMEM);
        s->max_delay = FFMAX(s->max_delay, d->delay);
    }
    if (!s->max_delay) {
        av_log(ctx, AV_LOG_ERROR, "At least one delay >0 must be specified.\n");
        return AVERROR(EINVAL);
    }
    switch (inlink->format) {
    case AV_SAMPLE_FMT_U8P : s->delay_channel = delay_channel_u8p ; break;
    case AV_SAMPLE_FMT_S16P: s->delay_channel = delay_channel_s16p; break;
    case AV_SAMPLE_FMT_S32P: s->delay_channel = delay_channel_s32p; break;
    case AV_SAMPLE_FMT_FLTP: s->delay_channel = delay_channel_fltp; break;
    case AV_SAMPLE_FMT_DBLP: s->delay_channel = delay_channel_dblp; break;
    }
    return 0;
}
