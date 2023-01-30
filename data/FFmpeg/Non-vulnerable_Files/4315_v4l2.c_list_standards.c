static void list_standards(AVFormatContext *ctx)
{
    int ret;
    struct video_data *s = ctx->priv_data;
    struct v4l2_standard standard;
    if (s->std_id == 0)
        return;
    for (standard.index = 0; ; standard.index++) {
        if (v4l2_ioctl(s->fd, VIDIOC_ENUMSTD, &standard) < 0) {
            ret = AVERROR(errno);
            if (ret == AVERROR(EINVAL)) {
                break;
            } else {
                av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_ENUMSTD): %s\n", av_err2str(ret));
                return;
            }
        }
        av_log(ctx, AV_LOG_INFO, "%2d, %16"PRIx64", %s\n",
               standard.index, (uint64_t)standard.id, standard.name);
    }
}
