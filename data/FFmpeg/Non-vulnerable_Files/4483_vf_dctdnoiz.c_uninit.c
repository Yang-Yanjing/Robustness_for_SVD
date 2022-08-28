static av_cold void uninit(AVFilterContext *ctx)
{
    int i;
    DCTdnoizContext *s = ctx->priv;
    av_freep(&s->weights);
    for (i = 0; i < 2; i++) {
        av_freep(&s->cbuf[i][0]);
        av_freep(&s->cbuf[i][1]);
        av_freep(&s->cbuf[i][2]);
    }
    for (i = 0; i < s->nb_threads; i++) {
        av_freep(&s->slices[i]);
        av_expr_free(s->expr[i]);
    }
}
