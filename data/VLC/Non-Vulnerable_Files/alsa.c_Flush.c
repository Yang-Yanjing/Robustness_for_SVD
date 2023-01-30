 */
static void Flush (audio_output_t *aout, bool wait)
{
    snd_pcm_t *pcm = aout->sys->pcm;
    if (wait)
        snd_pcm_drain (pcm);
    else
        snd_pcm_drop (pcm);
    snd_pcm_prepare (pcm);
}
