static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AStreamSyncContext *as = ctx->priv;
    int id = outlink == ctx->outputs[1];
    as->req[id]++;
    while (as->req[id] && !(as->eof & (1 << id))) {
        if (as->queue[as->next_out].nb) {
            send_next(ctx);
        } else {
            as->eof |= 1 << as->next_out;
            ff_request_frame(ctx->inputs[as->next_out]);
            if (as->eof & (1 << as->next_out))
                as->next_out = !as->next_out;
        }
    }
    return 0;
}
