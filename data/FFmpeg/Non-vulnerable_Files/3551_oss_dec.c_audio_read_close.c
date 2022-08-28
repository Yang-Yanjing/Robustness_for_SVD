static int audio_read_close(AVFormatContext *s1)
{
    OSSAudioData *s = s1->priv_data;
    ff_oss_audio_close(s);
    return 0;
}
