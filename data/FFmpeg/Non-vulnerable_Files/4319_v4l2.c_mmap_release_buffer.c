static void mmap_release_buffer(void *opaque, uint8_t *data)
{
    struct v4l2_buffer buf = { 0 };
    struct buff_data *buf_descriptor = opaque;
    struct video_data *s = buf_descriptor->s;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = buf_descriptor->index;
    av_free(buf_descriptor);
    enqueue_buffer(s, &buf);
}
