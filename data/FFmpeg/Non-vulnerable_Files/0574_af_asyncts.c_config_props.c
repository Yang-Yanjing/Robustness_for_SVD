static int config_props(AVFilterLink *link)
{
    ASyncContext *s = link->src->priv;
    int ret;
    s->min_delta = s->min_delta_sec * link->sample_rate;
    link->time_base = (AVRational){1, link->sample_rate};
    s->avr = avresample_alloc_context();
    if (!s->avr)
        return AVERROR(ENOMEM);
    av_opt_set_int(s->avr,  "in_channel_layout", link->channel_layout, 0);
    av_opt_set_int(s->avr, "out_channel_layout", link->channel_layout, 0);
    av_opt_set_int(s->avr,  "in_sample_fmt",     link->format,         0);
    av_opt_set_int(s->avr, "out_sample_fmt",     link->format,         0);
    av_opt_set_int(s->avr,  "in_sample_rate",    link->sample_rate,    0);
    av_opt_set_int(s->avr, "out_sample_rate",    link->sample_rate,    0);
    if (s->resample)
        av_opt_set_int(s->avr, "force_resampling", 1, 0);
    if ((ret = avresample_open(s->avr)) < 0)
        return ret;
    return 0;
}
