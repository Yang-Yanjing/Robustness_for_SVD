static av_cold void uninit(AVFilterContext *ctx)
{
    CoverContext *cover = ctx->priv;
    if (cover->cover_frame)
        av_freep(&cover->cover_frame->data[0]);
}
