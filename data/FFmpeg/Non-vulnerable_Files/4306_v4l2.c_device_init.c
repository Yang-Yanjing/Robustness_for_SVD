static int device_init(AVFormatContext *ctx, int *width, int *height,
                       uint32_t pixelformat)
{
    struct video_data *s = ctx->priv_data;
    struct v4l2_format fmt = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };
    int res = 0;
    fmt.fmt.pix.width = *width;
    fmt.fmt.pix.height = *height;
    fmt.fmt.pix.pixelformat = pixelformat;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    

    if (v4l2_ioctl(s->fd, VIDIOC_S_FMT, &fmt) < 0)
        res = AVERROR(errno);
    if ((*width != fmt.fmt.pix.width) || (*height != fmt.fmt.pix.height)) {
        av_log(ctx, AV_LOG_INFO,
               "The V4L2 driver changed the video from %dx%d to %dx%d\n",
               *width, *height, fmt.fmt.pix.width, fmt.fmt.pix.height);
        *width = fmt.fmt.pix.width;
        *height = fmt.fmt.pix.height;
    }
    if (pixelformat != fmt.fmt.pix.pixelformat) {
        av_log(ctx, AV_LOG_DEBUG,
               "The V4L2 driver changed the pixel format "
               "from 0x%08X to 0x%08X\n",
               pixelformat, fmt.fmt.pix.pixelformat);
        res = AVERROR(EINVAL);
    }
    if (fmt.fmt.pix.field == V4L2_FIELD_INTERLACED) {
        av_log(ctx, AV_LOG_DEBUG,
               "The V4L2 driver is using the interlaced mode\n");
        s->interlaced = 1;
    }
    return res;
}
