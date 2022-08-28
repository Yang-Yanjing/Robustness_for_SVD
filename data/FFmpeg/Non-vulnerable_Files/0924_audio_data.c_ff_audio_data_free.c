void ff_audio_data_free(AudioData **a)
{
    if (!*a)
        return;
    av_free((*a)->buffer);
    av_freep(a);
}
