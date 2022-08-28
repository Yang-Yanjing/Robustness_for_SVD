static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    SidechainCompressContext *s = ctx->priv;
    if (ctx->inputs[0]->sample_rate != ctx->inputs[1]->sample_rate) {
        av_log(ctx, AV_LOG_ERROR,
               "Inputs must have the same sample rate "
               "%d for in0 vs %d for in1\n",
               ctx->inputs[0]->sample_rate, ctx->inputs[1]->sample_rate);
        return AVERROR(EINVAL);
    }
    outlink->sample_rate = ctx->inputs[0]->sample_rate;
    outlink->time_base   = ctx->inputs[0]->time_base;
    outlink->channel_layout = ctx->inputs[0]->channel_layout;
    outlink->channels = ctx->inputs[0]->channels;
    s->attack_coeff = FFMIN(1.f, 1.f / (s->attack * outlink->sample_rate / 4000.f));
    s->release_coeff = FFMIN(1.f, 1.f / (s->release * outlink->sample_rate / 4000.f));
    return 0;
}
