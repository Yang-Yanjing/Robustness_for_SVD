static void guess_map_any(AVFilterContext *ctx, ChannelMap *ch,
                          uint64_t *inputs)
{
    int i;
    for (i = 0; i < ctx->nb_inputs; i++) {
        AVFilterLink *link = ctx->inputs[i];
        if ((inputs[i] & link->channel_layout) != link->channel_layout) {
            uint64_t unused = link->channel_layout & ~inputs[i];
            ch->input      = i;
            ch->in_channel = av_channel_layout_extract_channel(unused, 0);
            inputs[i]     |= ch->in_channel;
            return;
        }
    }
}
