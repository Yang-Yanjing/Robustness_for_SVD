av_cold int ff_alsa_close(AVFormatContext *s1)
{
    AlsaData *s = s1->priv_data;
    av_freep(&s->reorder_buf);
    if (CONFIG_ALSA_INDEV)
        ff_timefilter_destroy(s->timefilter);
    snd_pcm_close(s->h);
    return 0;
}
