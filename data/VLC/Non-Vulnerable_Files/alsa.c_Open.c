}
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->device = var_InheritString (aout, "alsa-audio-device");
    if (unlikely(sys->device == NULL))
        goto error;
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    aout_SoftVolumeInit (aout);
    aout->device_select = DeviceSelect;
    aout_DeviceReport (aout, sys->device);
    /* ALSA does not support hot-plug events so list devices at startup */
    char **ids, **names;
    int count = EnumDevices (VLC_OBJECT(aout), NULL, &ids, &names);
    if (count >= 0)
    {
        for (int i = 0; i < count; i++)
        {
            aout_HotplugReport (aout, ids[i], names[i]);
            free (names[i]);
            free (ids[i]);
        }
        free (names);
        free (ids);
    }
    return VLC_SUCCESS;
error:
    free (sys);
    return VLC_ENOMEM;
}
