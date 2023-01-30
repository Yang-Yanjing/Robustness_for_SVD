#pragma mark initialization
static int Open(vlc_object_t *obj)
{
    audio_output_t *p_aout = (audio_output_t *)obj;
    aout_sys_t *p_sys = malloc(sizeof (*p_sys));
    if (unlikely(p_sys == NULL))
        return VLC_ENOMEM;
    OSStatus err = noErr;
    vlc_mutex_init(&p_sys->device_list_lock);
    vlc_mutex_init(&p_sys->selected_device_lock);
    vlc_mutex_init(&p_sys->lock);
    vlc_cond_init(&p_sys->cond);
    p_sys->b_digital = false;
    p_sys->b_ignore_streams_changed_callback = false;
    p_sys->b_selected_dev_is_default = false;
    p_sys->b_paused = false;
    memset(&p_sys->sfmt_revert, 0, sizeof(p_sys->sfmt_revert));
    p_sys->i_stream_id = 0;
    p_aout->sys = p_sys;
    p_aout->start = Start;
    p_aout->stop = Stop;
    p_aout->volume_set = VolumeSet;
    p_aout->mute_set = MuteSet;
    p_aout->device_select = SwitchAudioDevice;
    p_sys->device_list = CFArrayCreate(kCFAllocatorDefault, NULL, 0, NULL);
    /*
     * Force an own run loop for callbacks.
     *
     * According to rtaudio, this is absolutely necessary since 10.6 to get correct notifications.
     * It might fix issues when using the module as a library where a proper loop is not setup already.
     */
    CFRunLoopRef theRunLoop = NULL;
    AudioObjectPropertyAddress property = { kAudioHardwarePropertyRunLoop, kAudioObjectPropertyScopeGlobal,kAudioObjectPropertyElementMaster };
    err = AudioObjectSetPropertyData(kAudioObjectSystemObject, &property, 0, NULL, sizeof(CFRunLoopRef), &theRunLoop);
    if (err != noErr) {
        msg_Err(p_aout, "failed to set the run loop property [%4.4s]", (char *)&err);
    }
    /* Attach a listener so that we are notified of a change in the device setup */
    AudioObjectPropertyAddress audioDevicesAddress = { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    err = AudioObjectAddPropertyListener(kAudioObjectSystemObject, &audioDevicesAddress, DevicesListener, (void *)p_aout);
    if (err != noErr)
        msg_Err(p_aout, "failed to add listener for audio device configuration [%4.4s]", (char *)&err);
    /* Attach a listener to be notified about changes in default audio device */
    AudioObjectPropertyAddress defaultDeviceAddress = { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    err = AudioObjectAddPropertyListener(kAudioObjectSystemObject, &defaultDeviceAddress, DefaultDeviceChangedListener, (void *)p_aout);
    if (err != noErr)
        msg_Err(p_aout, "failed to add listener for default audio device [%4.4s]", (char *)&err);
    RebuildDeviceList(p_aout);
    /* remember the volume */
    p_sys->f_volume = var_InheritInteger(p_aout, "auhal-volume") / (float)AOUT_VOLUME_DEFAULT;
    aout_VolumeReport(p_aout, p_sys->f_volume);
    p_sys->b_mute = var_InheritBool(p_aout, "mute");
    aout_MuteReport(p_aout, p_sys->b_mute);
    char *psz_audio_device = var_InheritString(p_aout, "auhal-audio-device");
    SwitchAudioDevice(p_aout, psz_audio_device);
    free(psz_audio_device);
    return VLC_SUCCESS;
}
