void ff_audio_resample_free(ResampleContext **c)
{
    if (!*c)
        return;
    ff_audio_data_free(&(*c)->buffer);
    av_free((*c)->filter_bank);
    av_freep(c);
}
