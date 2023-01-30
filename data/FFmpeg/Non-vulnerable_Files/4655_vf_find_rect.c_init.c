static av_cold int init(AVFilterContext *ctx)
{
    FOCContext *foc = ctx->priv;
    int ret, i;
    if (!foc->obj_filename) {
        av_log(ctx, AV_LOG_ERROR, "object filename not set\n");
        return AVERROR(EINVAL);
    }
    foc->obj_frame = av_frame_alloc();
    if (!foc->obj_frame)
        return AVERROR(ENOMEM);
    if ((ret = ff_load_image(foc->obj_frame->data, foc->obj_frame->linesize,
                             &foc->obj_frame->width, &foc->obj_frame->height,
                             &foc->obj_frame->format, foc->obj_filename, ctx)) < 0)
        return ret;
    if (foc->obj_frame->format != AV_PIX_FMT_GRAY8) {
        av_log(ctx, AV_LOG_ERROR, "object image is not a grayscale image\n");
        return AVERROR(EINVAL);
    }
    foc->needle_frame[0] = av_frame_clone(foc->obj_frame);
    for (i = 1; i < foc->mipmaps; i++) {
        foc->needle_frame[i] = downscale(foc->needle_frame[i-1]);
        if (!foc->needle_frame[i])
            return AVERROR(ENOMEM);
    }
    return 0;
}
