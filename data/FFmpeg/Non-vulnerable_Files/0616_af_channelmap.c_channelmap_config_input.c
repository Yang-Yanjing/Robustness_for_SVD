static int channelmap_config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    ChannelMapContext *s = ctx->priv;
    int nb_channels = av_get_channel_layout_nb_channels(inlink->channel_layout);
    int i, err = 0;
    const char *channel_name;
    char layout_name[256];
    for (i = 0; i < s->nch; i++) {
        struct ChannelMap *m = &s->map[i];
        if (s->mode == MAP_PAIR_STR_INT || s->mode == MAP_PAIR_STR_STR) {
            m->in_channel_idx = av_get_channel_layout_channel_index(
                inlink->channel_layout, m->in_channel);
        }
        if (m->in_channel_idx < 0 || m->in_channel_idx >= nb_channels) {
            av_get_channel_layout_string(layout_name, sizeof(layout_name),
                                         0, inlink->channel_layout);
            if (m->in_channel) {
                channel_name = av_get_channel_name(m->in_channel);
                av_log(ctx, AV_LOG_ERROR,
                       "input channel '%s' not available from input layout '%s'\n",
                       channel_name, layout_name);
            } else {
                av_log(ctx, AV_LOG_ERROR,
                       "input channel #%d not available from input layout '%s'\n",
                       m->in_channel_idx, layout_name);
            }
            err = AVERROR(EINVAL);
        }
    }
    return err;
}
