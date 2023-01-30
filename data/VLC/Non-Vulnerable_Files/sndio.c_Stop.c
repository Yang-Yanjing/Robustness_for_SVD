}
static void Stop (audio_output_t *aout)
{
    aout_sys_t *sys = aout->sys;
    sio_close (sys->hdl);
}
