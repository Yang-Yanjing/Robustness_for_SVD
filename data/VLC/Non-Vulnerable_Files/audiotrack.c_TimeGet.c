}
static int TimeGet(audio_output_t *p_aout, mtime_t *restrict delay)
{
    aout_sys_t *p_sys = p_aout->sys;
    uint32_t hal, dsp;
    if (!p_sys->at_getRenderPosition)
        return -1;
    if (p_sys->at_getRenderPosition(&hal, &dsp, MUSIC))
        return -1;
    if (p_sys->samples_written == 0) {
        p_sys->initial = dsp;
        return -1;
    }
    dsp -= p_sys->initial;
    if (dsp == 0)
        return -1;
    if (delay)
        *delay = ((mtime_t)p_sys->samples_written - dsp) * CLOCK_FREQ / p_sys->rate;
    return 0;
}
