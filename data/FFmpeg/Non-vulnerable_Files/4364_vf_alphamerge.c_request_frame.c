static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AlphaMergeContext *merge = ctx->priv;
    int in, ret;
    merge->frame_requested = 1;
    while (merge->frame_requested) {
        in = ff_bufqueue_peek(&merge->queue_main, 0) ? 1 : 0;
        ret = ff_request_frame(ctx->inputs[in]);
        if (ret < 0)
            return ret;
    }
    return 0;
}
