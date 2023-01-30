}
static void Flush (audio_output_t *aout, bool wait)
{
    aout_sys_t *sys = aout->sys;
    sio_stop (sys->hdl);
    sys->started = 0;
    sys->delay = 0;
    sio_start (sys->hdl);
    (void)wait;
}
