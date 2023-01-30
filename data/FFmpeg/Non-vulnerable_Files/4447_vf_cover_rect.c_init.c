static av_cold int init(AVFilterContext *ctx)
{
    CoverContext *cover = ctx->priv;
    int ret;
    if (cover->mode == MODE_COVER) {
        if (!cover->cover_filename) {
            av_log(ctx, AV_LOG_ERROR, "cover filename not set\n");
            return AVERROR(EINVAL);
        }
        cover->cover_frame = av_frame_alloc();
        if (!cover->cover_frame)
            return AVERROR(ENOMEM);
        if ((ret = ff_load_image(cover->cover_frame->data, cover->cover_frame->linesize,
                                &cover->cover_frame->width, &cover->cover_frame->height,
                                &cover->cover_frame->format, cover->cover_filename, ctx)) < 0)
            return ret;
        if (cover->cover_frame->format != AV_PIX_FMT_YUV420P && cover->cover_frame->format != AV_PIX_FMT_YUVJ420P) {
            av_log(ctx, AV_LOG_ERROR, "cover image is not a YUV420 image\n");
            return AVERROR(EINVAL);
        }
    }
    return 0;
}
