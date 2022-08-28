static av_cold void uninit(AVFilterContext *ctx)
{
    MergePlanesContext *s = ctx->priv;
    int i;
    ff_framesync_uninit(&s->fs);
    for (i = 0; i < ctx->nb_inputs; i++)
        av_freep(&ctx->input_pads[i].name);
}
