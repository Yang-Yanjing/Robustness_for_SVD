static av_cold void uninit(AVFilterContext *ctx)
{
    FOCContext *foc = ctx->priv;
    int i;
    for (i = 0; i < MAX_MIPMAPS; i++) {
        av_frame_free(&foc->needle_frame[i]);
        av_frame_free(&foc->haystack_frame[i]);
    }
    if (foc->obj_frame)
        av_freep(&foc->obj_frame->data[0]);
    av_frame_free(&foc->obj_frame);
}
