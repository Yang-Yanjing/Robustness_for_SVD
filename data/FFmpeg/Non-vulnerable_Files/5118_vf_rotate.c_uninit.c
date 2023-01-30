static av_cold void uninit(AVFilterContext *ctx)
{
    RotContext *rot = ctx->priv;
    av_expr_free(rot->angle_expr);
    rot->angle_expr = NULL;
}
