AVFILTER_DEFINE_CLASS(channelsplit);
static av_cold int init(AVFilterContext *ctx)
{
    ChannelSplitContext *s = ctx->priv;
    int nb_channels;
    int ret = 0, i;
    if (!(s->channel_layout = av_get_channel_layout(s->channel_layout_str))) {
        av_log(ctx, AV_LOG_ERROR, "Error parsing channel layout '%s'.\n",
               s->channel_layout_str);
        ret = AVERROR(EINVAL);
        goto fail;
    }
    nb_channels = av_get_channel_layout_nb_channels(s->channel_layout);
    for (i = 0; i < nb_channels; i++) {
        uint64_t channel = av_channel_layout_extract_channel(s->channel_layout, i);
        AVFilterPad pad  = { 0 };
        pad.type = AVMEDIA_TYPE_AUDIO;
        pad.name = av_get_channel_name(channel);
        ff_insert_outpad(ctx, i, &pad);
    }
fail:
    return ret;
}
