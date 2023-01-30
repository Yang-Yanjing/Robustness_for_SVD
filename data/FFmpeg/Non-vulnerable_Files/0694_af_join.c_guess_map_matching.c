static void guess_map_matching(AVFilterContext *ctx, ChannelMap *ch,
                               uint64_t *inputs)
{
    int i;
    for (i = 0; i < ctx->nb_inputs; i++) {
        AVFilterLink *link = ctx->inputs[i];
        if (ch->out_channel & link->channel_layout &&
            !(ch->out_channel & inputs[i])) {
            ch->input      = i;
            ch->in_channel = ch->out_channel;
            inputs[i]     |= ch->out_channel;
            return;
        }
    }
}
