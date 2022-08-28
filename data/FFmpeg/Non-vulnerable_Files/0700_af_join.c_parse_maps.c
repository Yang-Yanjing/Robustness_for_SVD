static int parse_maps(AVFilterContext *ctx)
{
    JoinContext *s = ctx->priv;
    char separator = '|';
    char *cur      = s->map;
#if FF_API_OLD_FILTER_OPTS
    if (cur && strchr(cur, ',')) {
        av_log(ctx, AV_LOG_WARNING, "This syntax is deprecated, use '|' to "
               "separate the mappings.\n");
        separator = ',';
    }
#endif
    while (cur && *cur) {
        char *sep, *next, *p;
        uint64_t in_channel = 0, out_channel = 0;
        int input_idx, out_ch_idx, in_ch_idx;
        next = strchr(cur, separator);
        if (next)
            *next++ = 0;
        
        if (!(sep = strchr(cur, '-'))) {
            av_log(ctx, AV_LOG_ERROR, "Missing separator '-' in channel "
                   "map '%s'\n", cur);
            return AVERROR(EINVAL);
        }
        *sep++ = 0;
#define PARSE_CHANNEL(str, var, inout)                                         \
        if (!(var = av_get_channel_layout(str))) {                             \
            av_log(ctx, AV_LOG_ERROR, "Invalid " inout " channel: %s.\n", str);\
            return AVERROR(EINVAL);                                            \
        }                                                                      \
        if (av_get_channel_layout_nb_channels(var) != 1) {                     \
            av_log(ctx, AV_LOG_ERROR, "Channel map describes more than one "   \
                   inout " channel.\n");                                       \
            return AVERROR(EINVAL);                                            \
        }
        
        PARSE_CHANNEL(sep, out_channel, "output");
        if (!(out_channel & s->channel_layout)) {
            av_log(ctx, AV_LOG_ERROR, "Output channel '%s' is not present in "
                   "requested channel layout.\n", sep);
            return AVERROR(EINVAL);
        }
        out_ch_idx = av_get_channel_layout_channel_index(s->channel_layout,
                                                         out_channel);
        if (s->channels[out_ch_idx].input >= 0) {
            av_log(ctx, AV_LOG_ERROR, "Multiple maps for output channel "
                   "'%s'.\n", sep);
            return AVERROR(EINVAL);
        }
        
        input_idx = strtol(cur, &cur, 0);
        if (input_idx < 0 || input_idx >= s->inputs) {
            av_log(ctx, AV_LOG_ERROR, "Invalid input stream index: %d.\n",
                   input_idx);
            return AVERROR(EINVAL);
        }
        if (*cur)
            cur++;
        in_ch_idx = strtol(cur, &p, 0);
        if (p == cur) {
            

            PARSE_CHANNEL(cur, in_channel, "input");
        }
        s->channels[out_ch_idx].input      = input_idx;
        if (in_channel)
            s->channels[out_ch_idx].in_channel = in_channel;
        else
            s->channels[out_ch_idx].in_channel_idx = in_ch_idx;
        cur = next;
    }
    return 0;
}
