static av_cold void uninit(AVFilterContext *ctx)
{
    DrawGraphContext *s = ctx->priv;
    int i;
    for (i = 0; i < 4; i++)
        av_expr_free(s->fg_expr[i]);
    av_frame_free(&s->out);
}
