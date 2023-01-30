static av_cold void fieldmatch_uninit(AVFilterContext *ctx)
{
    int i;
    FieldMatchContext *fm = ctx->priv;
    if (fm->prv != fm->src)
        av_frame_free(&fm->prv);
    if (fm->nxt != fm->src)
        av_frame_free(&fm->nxt);
    av_frame_free(&fm->src);
    av_freep(&fm->map_data[0]);
    av_freep(&fm->cmask_data[0]);
    av_freep(&fm->tbuffer);
    av_freep(&fm->c_array);
    for (i = 0; i < ctx->nb_inputs; i++)
        av_freep(&ctx->input_pads[i].name);
}
