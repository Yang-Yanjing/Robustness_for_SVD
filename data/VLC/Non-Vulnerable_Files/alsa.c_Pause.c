 */
static void Pause (audio_output_t *aout, bool pause, mtime_t date)
{
    snd_pcm_t *pcm = aout->sys->pcm;
    int val = snd_pcm_pause (pcm, pause);
    if (unlikely(val))
        PauseDummy (aout, pause, date);
}
