static void mmap_close(struct video_data *s)
{
    enum v4l2_buf_type type;
    int i;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    


    v4l2_ioctl(s->fd, VIDIOC_STREAMOFF, &type);
    for (i = 0; i < s->buffers; i++) {
        v4l2_munmap(s->buf_start[i], s->buf_len[i]);
    }
    av_freep(&s->buf_start);
    av_freep(&s->buf_len);
}
