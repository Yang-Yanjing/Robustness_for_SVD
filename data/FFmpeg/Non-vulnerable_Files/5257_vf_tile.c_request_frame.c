static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    TileContext *tile    = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    int r;
    r = ff_request_frame(inlink);
    if (r == AVERROR_EOF && tile->current)
        r = end_last_frame(ctx);
    return r;
}
