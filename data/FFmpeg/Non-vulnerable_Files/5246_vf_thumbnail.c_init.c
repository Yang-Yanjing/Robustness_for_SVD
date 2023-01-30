AVFILTER_DEFINE_CLASS(thumbnail);
static av_cold int init(AVFilterContext *ctx)
{
    ThumbContext *thumb = ctx->priv;
    thumb->frames = av_calloc(thumb->n_frames, sizeof(*thumb->frames));
    if (!thumb->frames) {
        av_log(ctx, AV_LOG_ERROR,
               "Allocation failure, try to lower the number of frames\n");
        return AVERROR(ENOMEM);
    }
    av_log(ctx, AV_LOG_VERBOSE, "batch size: %d frames\n", thumb->n_frames);
    return 0;
}
