static av_cold void uninit(AVFilterContext *ctx)
{
    Bs2bContext *bs2b = ctx->priv;
    if (bs2b->bs2bp)
        bs2b_close(bs2b->bs2bp);
}
