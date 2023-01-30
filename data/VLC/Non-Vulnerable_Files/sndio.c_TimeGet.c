}
static int TimeGet (audio_output_t *aout, mtime_t *restrict delay)
{
    aout_sys_t *sys = aout->sys;
    if (!sys->started)
	return -1;
    *delay = (mtime_t)sys->delay * CLOCK_FREQ / sys->rate;
    return 0;
}
