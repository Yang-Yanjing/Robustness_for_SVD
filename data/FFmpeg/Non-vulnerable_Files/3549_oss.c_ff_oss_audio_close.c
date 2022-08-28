int ff_oss_audio_close(OSSAudioData *s)
{
    close(s->fd);
    return 0;
}
