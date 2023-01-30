static int v4l_mm_read_picture(VideoData *s, uint8_t *buf)
{
    uint8_t *ptr;
    while (ioctl(s->fd, VIDIOCSYNC, &s->gb_frame) < 0 &&
           (errno == EAGAIN || errno == EINTR));
    ptr = s->video_buf + s->gb_buffers.offsets[s->gb_frame];
    memcpy(buf, ptr, s->frame_size);
    
    s->gb_buf.frame = s->gb_frame;
    if (ioctl(s->fd, VIDIOCMCAPTURE, &s->gb_buf) < 0) {
        if (errno == EAGAIN)
            av_log(NULL, AV_LOG_ERROR, "Cannot Sync\n");
        else
            av_log(NULL, AV_LOG_ERROR, "VIDIOCMCAPTURE: %s\n", strerror(errno));
        return AVERROR(EIO);
    }
    
    s->gb_frame = (s->gb_frame + 1) % s->gb_buffers.frames;
    return s->frame_size;
}
