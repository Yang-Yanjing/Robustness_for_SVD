static av_cold void uninit(AVFilterContext *ctx)
{
    StackContext *s = ctx->priv;
    ff_framesync_uninit(&s->fs);
    av_freep(&s->frames);
}
