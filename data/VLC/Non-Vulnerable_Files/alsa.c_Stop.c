 */
static void Stop (audio_output_t *aout)
{
    aout_sys_t *sys = aout->sys;
    snd_pcm_t *pcm = sys->pcm;
    snd_pcm_drop (pcm);
    snd_pcm_close (pcm);
}
