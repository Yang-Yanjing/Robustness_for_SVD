static av_cold void pp_uninit(AVFilterContext *ctx)
{
    int i;
    PPFilterContext *pp = ctx->priv;
    for (i = 0; i <= PP_QUALITY_MAX; i++)
        pp_free_mode(pp->modes[i]);
    if (pp->pp_ctx)
        pp_free_context(pp->pp_ctx);
}
