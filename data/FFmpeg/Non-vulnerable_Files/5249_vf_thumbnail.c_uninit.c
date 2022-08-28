static av_cold void uninit(AVFilterContext *ctx)
{
    int i;
    ThumbContext *thumb = ctx->priv;
    for (i = 0; i < thumb->n_frames && thumb->frames[i].buf; i++)
        av_frame_free(&thumb->frames[i].buf);
    av_freep(&thumb->frames);
}
