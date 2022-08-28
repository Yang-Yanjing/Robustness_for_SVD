static av_cold void uninit(AVFilterContext *ctx)
{
    ResampleContext *s = ctx->priv;
    if (s->avr) {
        avresample_close(s->avr);
        avresample_free(&s->avr);
    }
    av_dict_free(&s->options);
}
