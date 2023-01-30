static av_cold void uninit(AVFilterContext *ctx)
{
    OverlayContext *s = ctx->priv;
    ff_dualinput_uninit(&s->dinput);
    av_expr_free(s->x_pexpr); s->x_pexpr = NULL;
    av_expr_free(s->y_pexpr); s->y_pexpr = NULL;
}
