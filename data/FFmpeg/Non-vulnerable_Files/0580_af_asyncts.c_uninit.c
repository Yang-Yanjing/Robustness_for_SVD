static av_cold void uninit(AVFilterContext *ctx)
{
    ASyncContext *s = ctx->priv;
    if (s->avr) {
        avresample_close(s->avr);
        avresample_free(&s->avr);
    }
}
