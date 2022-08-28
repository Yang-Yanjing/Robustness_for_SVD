static av_cold void uninit(AVFilterContext *ctx)
{
    int i;
    for (i = 0; i < ctx->nb_outputs; i++)
        av_freep(&ctx->output_pads[i].name);
}
