static int request_frame(AVFilterLink *outlink)
{
    int ret;
    AVFilterContext *ctx = outlink->src;
    DecimateContext *dm = ctx->priv;
    const uint32_t eof_mask = 1<<INPUT_MAIN | dm->ppsrc<<INPUT_CLEANSRC;
    if ((dm->eof & eof_mask) == eof_mask) 
        return AVERROR_EOF;
    if ((ret = request_inlink(ctx, INPUT_MAIN)) < 0)
        return ret;
    if (dm->ppsrc && (ret = request_inlink(ctx, INPUT_CLEANSRC)) < 0)
        return ret;
    return 0;
}
