static int mmap_start(AVFormatContext *ctx)
{
    struct video_data *s = ctx->priv_data;
    enum v4l2_buf_type type;
    int i, res;
    for (i = 0; i < s->buffers; i++) {
        struct v4l2_buffer buf = {
            .type   = V4L2_BUF_TYPE_VIDEO_CAPTURE,
            .index  = i,
            .memory = V4L2_MEMORY_MMAP
        };
        if (v4l2_ioctl(s->fd, VIDIOC_QBUF, &buf) < 0) {
            res = AVERROR(errno);
            av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_QBUF): %s\n",
                   av_err2str(res));
            return res;
        }
    }
    s->buffers_queued = s->buffers;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (v4l2_ioctl(s->fd, VIDIOC_STREAMON, &type) < 0) {
        res = AVERROR(errno);
        av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_STREAMON): %s\n",
               av_err2str(res));
        return res;
    }
    return 0;
}
