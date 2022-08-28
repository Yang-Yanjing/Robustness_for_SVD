static av_cold void uninit(AVFilterContext *ctx)
{
    NoiseContext *n = ctx->priv;
    int i;
    for (i = 0; i < 4; i++)
        av_freep(&n->param[i].noise);
}
