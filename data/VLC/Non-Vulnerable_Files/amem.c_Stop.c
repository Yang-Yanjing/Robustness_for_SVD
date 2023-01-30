}
static void Stop (audio_output_t *aout)
{
    aout_sys_t *sys = aout->sys;
    if (sys->cleanup != NULL)
        sys->cleanup (sys->opaque);
    sys->ready = false;
}
