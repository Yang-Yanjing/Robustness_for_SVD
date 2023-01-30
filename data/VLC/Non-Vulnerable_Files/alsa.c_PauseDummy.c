}
static void PauseDummy (audio_output_t *aout, bool pause, mtime_t date)
{
    snd_pcm_t *pcm = aout->sys->pcm;
    /* Stupid device cannot pause. Discard samples. */
    if (pause)
        snd_pcm_drop (pcm);
    else
        snd_pcm_prepare (pcm);
    (void) date;
}
