static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    AStreamSyncContext *as = ctx->priv;
    int id = inlink == ctx->inputs[1];
    as->queue[id].buf[(as->queue[id].tail + as->queue[id].nb++) % QUEUE_SIZE] =
        insamples;
    as->eof &= ~(1 << id);
    send_next(ctx);
    return 0;
}
