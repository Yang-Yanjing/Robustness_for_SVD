static int device_open(AVFormatContext *ctx)
{
    struct video_data *s = ctx->priv_data;
    struct v4l2_capability cap;
    int fd;
    int err;
    int flags = O_RDWR;
#define SET_WRAPPERS(prefix) do {       \
    s->open_f   = prefix ## open;       \
    s->close_f  = prefix ## close;      \
    s->dup_f    = prefix ## dup;        \
    s->ioctl_f  = prefix ## ioctl;      \
    s->read_f   = prefix ## read;       \
    s->mmap_f   = prefix ## mmap;       \
    s->munmap_f = prefix ## munmap;     \
} while (0)
    if (s->use_libv4l2) {
#if CONFIG_LIBV4L2
        SET_WRAPPERS(v4l2_);
#else
        av_log(ctx, AV_LOG_ERROR, "libavdevice is not build with libv4l2 support.\n");
        return AVERROR(EINVAL);
#endif
    } else {
        SET_WRAPPERS();
    }
#define v4l2_open   s->open_f
#define v4l2_close  s->close_f
#define v4l2_dup    s->dup_f
#define v4l2_ioctl  s->ioctl_f
#define v4l2_read   s->read_f
#define v4l2_mmap   s->mmap_f
#define v4l2_munmap s->munmap_f
    if (ctx->flags & AVFMT_FLAG_NONBLOCK) {
        flags |= O_NONBLOCK;
    }
    fd = v4l2_open(ctx->filename, flags, 0);
    if (fd < 0) {
        err = AVERROR(errno);
        av_log(ctx, AV_LOG_ERROR, "Cannot open video device %s: %s\n",
               ctx->filename, av_err2str(err));
        return err;
    }
    if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        err = AVERROR(errno);
        av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_QUERYCAP): %s\n",
               av_err2str(err));
        goto fail;
    }
    av_log(ctx, AV_LOG_VERBOSE, "fd:%d capabilities:%x\n",
           fd, cap.capabilities);
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        av_log(ctx, AV_LOG_ERROR, "Not a video capture device.\n");
        err = AVERROR(ENODEV);
        goto fail;
    }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        av_log(ctx, AV_LOG_ERROR,
               "The device does not support the streaming I/O method.\n");
        err = AVERROR(ENOSYS);
        goto fail;
    }
    return fd;
fail:
    v4l2_close(fd);
    return err;
}
