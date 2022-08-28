static void uninit(AVFilterContext *ctx)
{
    EQContext *eq = ctx->priv;
    av_expr_free(eq->contrast_pexpr);     eq->contrast_pexpr     = NULL;
    av_expr_free(eq->brightness_pexpr);   eq->brightness_pexpr   = NULL;
    av_expr_free(eq->saturation_pexpr);   eq->saturation_pexpr   = NULL;
    av_expr_free(eq->gamma_pexpr);        eq->gamma_pexpr        = NULL;
    av_expr_free(eq->gamma_weight_pexpr); eq->gamma_weight_pexpr = NULL;
    av_expr_free(eq->gamma_r_pexpr);      eq->gamma_r_pexpr      = NULL;
    av_expr_free(eq->gamma_g_pexpr);      eq->gamma_g_pexpr      = NULL;
    av_expr_free(eq->gamma_b_pexpr);      eq->gamma_b_pexpr      = NULL;
}
