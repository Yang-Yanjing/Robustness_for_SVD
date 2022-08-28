static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AMergeContext *s = ctx->priv;
    AVBPrint bp;
    int i;
    for (i = 1; i < s->nb_inputs; i++) {
        if (ctx->inputs[i]->sample_rate != ctx->inputs[0]->sample_rate) {
            av_log(ctx, AV_LOG_ERROR,
                   "Inputs must have the same sample rate "
                   "%d for in%d vs %d\n",
                   ctx->inputs[i]->sample_rate, i, ctx->inputs[0]->sample_rate);
            return AVERROR(EINVAL);
        }
    }
    s->bps = av_get_bytes_per_sample(ctx->outputs[0]->format);
    outlink->sample_rate = ctx->inputs[0]->sample_rate;
    outlink->time_base   = ctx->inputs[0]->time_base;
    av_bprint_init(&bp, 0, 1);
    for (i = 0; i < s->nb_inputs; i++) {
        av_bprintf(&bp, "%sin%d:", i ? " + " : "", i);
        av_bprint_channel_layout(&bp, -1, ctx->inputs[i]->channel_layout);
    }
    av_bprintf(&bp, " -> out:");
    av_bprint_channel_layout(&bp, -1, ctx->outputs[0]->channel_layout);
    av_log(ctx, AV_LOG_VERBOSE, "%s\n", bp.str);
    return 0;
}
