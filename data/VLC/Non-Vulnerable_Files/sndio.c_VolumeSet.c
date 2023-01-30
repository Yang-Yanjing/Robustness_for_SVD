}
static int VolumeSet (audio_output_t *aout, float fvol)
{
    aout_sys_t *sys = aout->sys;
    unsigned volume;
    if (fvol < 0)
	fvol = 0;
    if (fvol > 1)
	fvol = 1;
    volume = lroundf (fvol * SIO_MAXVOL);
    if (!sys->mute && !sio_setvol (sys->hdl, volume))
        return -1;
    sys->volume = volume;
    return 0;
}
