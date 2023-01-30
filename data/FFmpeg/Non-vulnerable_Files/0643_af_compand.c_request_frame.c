static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    CompandContext *s    = ctx->priv;
    int ret = 0;
    ret = ff_request_frame(ctx->inputs[0]);
    if (ret == AVERROR_EOF && !ctx->is_disabled && s->delay_count)
        ret = compand_drain(outlink);
    return ret;
}
