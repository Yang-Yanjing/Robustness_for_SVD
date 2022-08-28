static av_cold void decimate_uninit(AVFilterContext *ctx)
{
    int i;
    DecimateContext *dm = ctx->priv;
    av_frame_free(&dm->last);
    av_freep(&dm->bdiffs);
    av_freep(&dm->queue);
    av_freep(&dm->clean_src);
    for (i = 0; i < ctx->nb_inputs; i++)
        av_freep(&ctx->input_pads[i].name);
}
