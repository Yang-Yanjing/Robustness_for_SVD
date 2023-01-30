void ff_audio_convert_free(AudioConvert **ac)
{
    if (!*ac)
        return;
    ff_dither_free(&(*ac)->dc);
    av_freep(ac);
}
