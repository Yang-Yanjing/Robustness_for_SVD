}
static int TimeGet (audio_output_t *aout, mtime_t *restrict delay)
{
    aout_sys_t *sys = aout->sys;
    snd_pcm_sframes_t frames;
    int val = snd_pcm_delay (sys->pcm, &frames);
    if (val)
    {
        msg_Err (aout, "cannot estimate delay: %s", snd_strerror (val));
        return -1;
    }
    *delay = frames * CLOCK_FREQ / sys->rate;
    return 0;
}
