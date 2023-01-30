static int grab_read_close(AVFormatContext *s1)
{
    VideoData *s = s1->priv_data;
    int c;
    c = METEOR_CAP_STOP_CONT;
    ioctl(s->video_fd, METEORCAPTUR, &c);
    close(s->video_fd);
    c = AUDIO_MUTE;
    ioctl(s->tuner_fd, BT848_SAUDIO, &c);
    close(s->tuner_fd);
    munmap((caddr_t)video_buf, video_buf_size);
    return 0;
}
