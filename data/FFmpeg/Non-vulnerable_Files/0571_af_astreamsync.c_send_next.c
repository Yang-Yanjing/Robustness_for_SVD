static void send_next(AVFilterContext *ctx)
{
    AStreamSyncContext *as = ctx->priv;
    int i;
    while (1) {
        if (!as->queue[as->next_out].nb)
            break;
        send_out(ctx, as->next_out);
        if (!as->eof)
            as->next_out = av_expr_eval(as->expr, as->var_values, NULL) >= 0;
    }
    for (i = 0; i < 2; i++)
        if (as->queue[i].nb == QUEUE_SIZE)
            send_out(ctx, i);
}
