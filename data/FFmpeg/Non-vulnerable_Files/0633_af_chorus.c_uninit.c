static av_cold void uninit(AVFilterContext *ctx)
{
    ChorusContext *s = ctx->priv;
    int n;
    av_freep(&s->delays);
    av_freep(&s->decays);
    av_freep(&s->speeds);
    av_freep(&s->depths);
    if (s->chorusbuf)
        av_freep(&s->chorusbuf[0]);
    av_freep(&s->chorusbuf);
    if (s->phase)
        for (n = 0; n < s->channels; n++)
            av_freep(&s->phase[n]);
    av_freep(&s->phase);
    av_freep(&s->counter);
    av_freep(&s->length);
    if (s->lookup_table)
        for (n = 0; n < s->num_chorus; n++)
            av_freep(&s->lookup_table[n]);
    av_freep(&s->lookup_table);
}
