static av_cold int audio_read_close(AVFormatContext *s1)
{
    SndioData *s = s1->priv_data;
    ff_sndio_close(s);
    return 0;
}
