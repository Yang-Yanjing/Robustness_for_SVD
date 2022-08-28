static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    BlackDetectContext *blackdetect = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    int ret = ff_request_frame(inlink);
    if (ret == AVERROR_EOF && blackdetect->black_started) {
        
        blackdetect->black_end = blackdetect->last_picref_pts;
        check_black_end(ctx);
    }
    return ret;
}
