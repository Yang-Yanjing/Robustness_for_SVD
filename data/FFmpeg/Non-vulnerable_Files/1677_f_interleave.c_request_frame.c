static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    InterleaveContext *s = ctx->priv;
    int i, ret;
    for (i = 0; i < ctx->nb_inputs; i++) {
        if (!s->queues[i].available && !ctx->inputs[i]->closed) {
            ret = ff_request_frame(ctx->inputs[i]);
            if (ret != AVERROR_EOF)
                return ret;
        }
    }
    return push_frame(ctx);
}
