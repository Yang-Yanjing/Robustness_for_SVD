static void list_framesizes(AVFormatContext *ctx, uint32_t pixelformat)
{
    const struct video_data *s = ctx->priv_data;
    struct v4l2_frmsizeenum vfse = { .pixel_format = pixelformat };
    while(!v4l2_ioctl(s->fd, VIDIOC_ENUM_FRAMESIZES, &vfse)) {
        switch (vfse.type) {
        case V4L2_FRMSIZE_TYPE_DISCRETE:
            av_log(ctx, AV_LOG_INFO, " %ux%u",
                   vfse.discrete.width, vfse.discrete.height);
        break;
        case V4L2_FRMSIZE_TYPE_CONTINUOUS:
        case V4L2_FRMSIZE_TYPE_STEPWISE:
            av_log(ctx, AV_LOG_INFO, " {%u-%u, %u}x{%u-%u, %u}",
                   vfse.stepwise.min_width,
                   vfse.stepwise.max_width,
                   vfse.stepwise.step_width,
                   vfse.stepwise.min_height,
                   vfse.stepwise.max_height,
                   vfse.stepwise.step_height);
        }
        vfse.index++;
    }
}
