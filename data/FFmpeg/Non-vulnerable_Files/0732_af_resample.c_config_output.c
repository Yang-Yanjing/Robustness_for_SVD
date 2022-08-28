static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    ResampleContext   *s = ctx->priv;
    char buf1[64], buf2[64];
    int ret;
    int64_t resampling_forced;
    if (s->avr) {
        avresample_close(s->avr);
        avresample_free(&s->avr);
    }
    if (inlink->channel_layout == outlink->channel_layout &&
        inlink->sample_rate    == outlink->sample_rate    &&
        (inlink->format        == outlink->format ||
        (av_get_channel_layout_nb_channels(inlink->channel_layout)  == 1 &&
         av_get_channel_layout_nb_channels(outlink->channel_layout) == 1 &&
         av_get_planar_sample_fmt(inlink->format) ==
         av_get_planar_sample_fmt(outlink->format))))
        return 0;
    if (!(s->avr = avresample_alloc_context()))
        return AVERROR(ENOMEM);
    if (s->options) {
        int ret;
        AVDictionaryEntry *e = NULL;
        while ((e = av_dict_get(s->options, "", e, AV_DICT_IGNORE_SUFFIX)))
            av_log(ctx, AV_LOG_VERBOSE, "lavr option: %s=%s\n", e->key, e->value);
        ret = av_opt_set_dict(s->avr, &s->options);
        if (ret < 0)
            return ret;
    }
    av_opt_set_int(s->avr,  "in_channel_layout", inlink ->channel_layout, 0);
    av_opt_set_int(s->avr, "out_channel_layout", outlink->channel_layout, 0);
    av_opt_set_int(s->avr,  "in_sample_fmt",     inlink ->format,         0);
    av_opt_set_int(s->avr, "out_sample_fmt",     outlink->format,         0);
    av_opt_set_int(s->avr,  "in_sample_rate",    inlink ->sample_rate,    0);
    av_opt_set_int(s->avr, "out_sample_rate",    outlink->sample_rate,    0);
    if ((ret = avresample_open(s->avr)) < 0)
        return ret;
    av_opt_get_int(s->avr, "force_resampling", 0, &resampling_forced);
    s->resampling = resampling_forced || (inlink->sample_rate != outlink->sample_rate);
    if (s->resampling) {
        outlink->time_base = (AVRational){ 1, outlink->sample_rate };
        s->next_pts        = AV_NOPTS_VALUE;
        s->next_in_pts     = AV_NOPTS_VALUE;
    } else
        outlink->time_base = inlink->time_base;
    av_get_channel_layout_string(buf1, sizeof(buf1),
                                 -1, inlink ->channel_layout);
    av_get_channel_layout_string(buf2, sizeof(buf2),
                                 -1, outlink->channel_layout);
    av_log(ctx, AV_LOG_VERBOSE,
           "fmt:%s srate:%d cl:%s -> fmt:%s srate:%d cl:%s\n",
           av_get_sample_fmt_name(inlink ->format), inlink ->sample_rate, buf1,
           av_get_sample_fmt_name(outlink->format), outlink->sample_rate, buf2);
    return 0;
}
