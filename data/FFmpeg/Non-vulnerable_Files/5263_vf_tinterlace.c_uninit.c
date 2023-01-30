static av_cold void uninit(AVFilterContext *ctx)
{
    TInterlaceContext *tinterlace = ctx->priv;
    av_frame_free(&tinterlace->cur );
    av_frame_free(&tinterlace->next);
    av_freep(&tinterlace->black_data[0]);
}
