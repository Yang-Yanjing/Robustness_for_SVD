static av_cold void uninit(AVFilterContext *ctx)
{
    DebandContext *s = ctx->priv;
    av_freep(&s->x_pos);
    av_freep(&s->y_pos);
}
