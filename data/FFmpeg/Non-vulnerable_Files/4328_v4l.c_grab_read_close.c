static int grab_read_close(AVFormatContext *s1)
{
    VideoData *s = s1->priv_data;
    if (s->use_mmap)
        munmap(s->video_buf, s->gb_buffers.size);
    

    s->audio_saved.flags |= VIDEO_AUDIO_MUTE;
    ioctl(s->fd, VIDIOCSAUDIO, &s->audio_saved);
    close(s->fd);
    return 0;
}
