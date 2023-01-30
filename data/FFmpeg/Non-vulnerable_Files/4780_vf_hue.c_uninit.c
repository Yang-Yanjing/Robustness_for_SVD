static av_cold void uninit(AVFilterContext *ctx)
{
    HueContext *hue = ctx->priv;
    av_expr_free(hue->brightness_pexpr);
    av_expr_free(hue->hue_deg_pexpr);
    av_expr_free(hue->hue_pexpr);
    av_expr_free(hue->saturation_pexpr);
}
