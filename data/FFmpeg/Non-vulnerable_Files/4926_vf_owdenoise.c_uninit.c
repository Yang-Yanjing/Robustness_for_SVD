static av_cold void uninit(AVFilterContext *ctx)
{
    int i, j;
    OWDenoiseContext *s = ctx->priv;
    for (j = 0; j < 4; j++)
        for (i = 0; i <= s->depth; i++)
            av_freep(&s->plane[i][j]);
}
