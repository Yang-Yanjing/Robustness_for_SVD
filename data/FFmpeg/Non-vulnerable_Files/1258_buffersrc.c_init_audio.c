AVFILTER_DEFINE_CLASS(abuffer);
static av_cold int init_audio(AVFilterContext *ctx)
{
    BufferSourceContext *s = ctx->priv;
    int ret = 0;
    if (s->sample_fmt == AV_SAMPLE_FMT_NONE) {
        av_log(ctx, AV_LOG_ERROR, "Sample format was not set or was invalid\n");
        return AVERROR(EINVAL);
    }
    if (s->channel_layout_str) {
        int n;
        s->channel_layout = av_get_channel_layout(s->channel_layout_str);
        if (!s->channel_layout) {
            av_log(ctx, AV_LOG_ERROR, "Invalid channel layout %s.\n",
                   s->channel_layout_str);
            return AVERROR(EINVAL);
        }
        n = av_get_channel_layout_nb_channels(s->channel_layout);
        if (s->channels) {
            if (n != s->channels) {
                av_log(ctx, AV_LOG_ERROR,
                       "Mismatching channel count %d and layout '%s' "
                       "(%d channels)\n",
                       s->channels, s->channel_layout_str, n);
                return AVERROR(EINVAL);
            }
        }
        s->channels = n;
    } else if (!s->channels) {
        av_log(ctx, AV_LOG_ERROR, "Neither number of channels nor "
                                  "channel layout specified\n");
        return AVERROR(EINVAL);
    }
    if (!(s->fifo = av_fifo_alloc(sizeof(AVFrame*))))
        return AVERROR(ENOMEM);
    if (!s->time_base.num)
        s->time_base = (AVRational){1, s->sample_rate};
    av_log(ctx, AV_LOG_VERBOSE,
           "tb:%d/%d samplefmt:%s samplerate:%d chlayout:%s\n",
           s->time_base.num, s->time_base.den, av_get_sample_fmt_name(s->sample_fmt),
           s->sample_rate, s->channel_layout_str);
    s->warning_limit = 100;
    return ret;
}
