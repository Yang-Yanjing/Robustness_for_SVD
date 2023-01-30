static av_cold void uninit(AVFilterContext *ctx)
{
    CropContext *s = ctx->priv;
    av_expr_free(s->x_pexpr);
    s->x_pexpr = NULL;
    av_expr_free(s->y_pexpr);
    s->y_pexpr = NULL;
}
