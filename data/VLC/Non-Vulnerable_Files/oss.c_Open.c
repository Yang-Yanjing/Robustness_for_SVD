}
static int Open (vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc (sizeof (*sys));
    if(unlikely( sys == NULL ))
        return VLC_ENOMEM;
    sys->fd = -1;
    sys->device = var_InheritString (aout, "oss-audio-device");
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    aout->device_select = DeviceSelect;
    aout_DeviceReport (aout, sys->device);
    aout_SoftVolumeInit (aout);
    DevicesEnum (aout);
    return VLC_SUCCESS;
}
