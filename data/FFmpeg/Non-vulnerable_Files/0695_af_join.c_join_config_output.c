static int join_config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    JoinContext       *s = ctx->priv;
    uint64_t *inputs;   
    int i, ret = 0;
    
    if (!(inputs = av_mallocz_array(ctx->nb_inputs, sizeof(*inputs))))
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_channels; i++) {
        ChannelMap *ch = &s->channels[i];
        AVFilterLink *inlink;
        if (ch->input < 0)
            continue;
        inlink = ctx->inputs[ch->input];
        if (!ch->in_channel)
            ch->in_channel = av_channel_layout_extract_channel(inlink->channel_layout,
                                                               ch->in_channel_idx);
        if (!(ch->in_channel & inlink->channel_layout)) {
            av_log(ctx, AV_LOG_ERROR, "Requested channel %s is not present in "
                   "input stream #%d.\n", av_get_channel_name(ch->in_channel),
                   ch->input);
            ret = AVERROR(EINVAL);
            goto fail;
        }
        inputs[ch->input] |= ch->in_channel;
    }
    
    
    for (i = 0; i < s->nb_channels; i++) {
        ChannelMap *ch = &s->channels[i];
        if (ch->input < 0)
            guess_map_matching(ctx, ch, inputs);
    }
    
    for (i = 0; i < s->nb_channels; i++) {
        ChannelMap *ch = &s->channels[i];
        if (ch->input < 0)
            guess_map_any(ctx, ch, inputs);
        if (ch->input < 0) {
            av_log(ctx, AV_LOG_ERROR, "Could not find input channel for "
                   "output channel '%s'.\n",
                   av_get_channel_name(ch->out_channel));
            goto fail;
        }
        ch->in_channel_idx = av_get_channel_layout_channel_index(ctx->inputs[ch->input]->channel_layout,
                                                                 ch->in_channel);
    }
    
    av_log(ctx, AV_LOG_VERBOSE, "mappings: ");
    for (i = 0; i < s->nb_channels; i++) {
        ChannelMap *ch = &s->channels[i];
        av_log(ctx, AV_LOG_VERBOSE, "%d.%s => %s ", ch->input,
               av_get_channel_name(ch->in_channel),
               av_get_channel_name(ch->out_channel));
    }
    av_log(ctx, AV_LOG_VERBOSE, "\n");
    for (i = 0; i < ctx->nb_inputs; i++) {
        if (!inputs[i])
            av_log(ctx, AV_LOG_WARNING, "No channels are used from input "
                   "stream %d.\n", i);
    }
fail:
    av_freep(&inputs);
    return ret;
}
