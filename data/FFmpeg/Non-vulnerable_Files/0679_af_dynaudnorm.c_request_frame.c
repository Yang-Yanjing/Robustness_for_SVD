static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    DynamicAudioNormalizerContext *s = ctx->priv;
    int ret = 0;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && !ctx->is_disabled && s->delay)
        ret = flush_buffer(s, ctx->inputs[0], outlink);
    return ret;
}
