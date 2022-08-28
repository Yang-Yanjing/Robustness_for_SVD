static av_cold void uninit(AVFilterContext *ctx)
{
    VignetteContext *s = ctx->priv;
    av_freep(&s->fmap);
    av_expr_free(s->angle_pexpr);
    av_expr_free(s->x0_pexpr);
    av_expr_free(s->y0_pexpr);
}
