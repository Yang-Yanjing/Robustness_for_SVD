}
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    HINSTANCE hdsound_dll = LoadLibrary(_T("DSOUND.DLL"));
    if (hdsound_dll == NULL)
    {
        msg_Warn(aout, "cannot open DSOUND.DLL");
        return VLC_EGENERIC;
    }
    aout_sys_t *sys = calloc(1, sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->hdsound_dll = hdsound_dll;
    aout->sys = sys;
    aout->start = OutputStart;
    aout->stop = OutputStop;
    aout->volume_set = VolumeSet;
    aout->mute_set = MuteSet;
    aout->device_select = DeviceSelect;
    /* Volume */
    sys->volume.volume = var_InheritFloat(aout, "directx-volume");
    aout_VolumeReport(aout, sys->volume.volume );
    MuteSet(aout, var_InheritBool(aout, "mute"));
    /* DirectSound does not support hot-plug events (unless with WASAPI) */
    char **ids, **names;
    int count = ReloadDirectXDevices(obj, NULL, &ids, &names);
    if (count >= 0)
    {
        for (int i = 0; i < count; i++)
        {
            aout_HotplugReport(aout, ids[i], names[i]);
            free(names[i]);
            free(ids[i]);
        }
        free(names);
        free(ids);
    }
    char *dev = var_CreateGetNonEmptyString(aout, "directx-audio-device");
    aout_DeviceReport(aout, dev);
    free(dev);
    return VLC_SUCCESS;
}
