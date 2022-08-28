static av_cold void uninit(AVFilterContext *ctx)
{
    LutContext *s = ctx->priv;
    int i;
    for (i = 0; i < 4; i++) {
        av_expr_free(s->comp_expr[i]);
        s->comp_expr[i] = NULL;
        av_freep(&s->comp_expr_str[i]);
    }
}
