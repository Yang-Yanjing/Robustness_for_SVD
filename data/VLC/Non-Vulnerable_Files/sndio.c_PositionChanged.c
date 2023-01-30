}
static void PositionChanged (void *arg, int delta)
{
    audio_output_t *aout = arg;
    aout_sys_t *sys = aout->sys;
    sys->delay -= delta;
    sys->started = 1;
}
