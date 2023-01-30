static int enqueue_buffer(struct video_data *s, struct v4l2_buffer *buf)
{
    int res = 0;
    if (v4l2_ioctl(s->fd, VIDIOC_QBUF, buf) < 0) {
        res = AVERROR(errno);
        av_log(NULL, AV_LOG_ERROR, "ioctl(VIDIOC_QBUF): %s\n", av_err2str(res));
    } else {
        avpriv_atomic_int_add_and_fetch(&s->buffers_queued, 1);
    }
    return res;
}
