static av_cold void uninit(AVFilterContext *ctx)
{
    AStreamSyncContext *as = ctx->priv;
    av_expr_free(as->expr);
    as->expr = NULL;
}
