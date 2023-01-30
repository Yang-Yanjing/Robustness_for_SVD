}
static void Close(vlc_object_t *obj)
{
    audio_output_t *p_aout = (audio_output_t *)obj;
    aout_sys_t *p_sys = p_aout->sys;
    OSStatus err = noErr;
    /* remove audio devices callback */
    AudioObjectPropertyAddress audioDevicesAddress = { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    err = AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &audioDevicesAddress, DevicesListener, (void *)p_aout);
    if (err != noErr)
        msg_Err(p_aout, "AudioHardwareRemovePropertyListener failed [%4.4s]", (char *)&err);
    /* remove listener to be notified about changes in default audio device */
    AudioObjectPropertyAddress defaultDeviceAddress = { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    err = AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &defaultDeviceAddress, DefaultDeviceChangedListener, (void *)p_aout);
    if (err != noErr)
        msg_Err(p_aout, "failed to remove listener for default audio device [%4.4s]", (char *)&err);
    /*
     * StreamsChangedListener can rebuild the device list and thus held the device_list_lock.
     * To avoid a possible deadlock, an array copy is created here.
     * In rare cases, this can lead to missing StreamsChangedListener callback deregistration (TODO).
     */
    vlc_mutex_lock(&p_sys->device_list_lock);
    CFArrayRef device_list_cpy = CFArrayCreateCopy(NULL, p_sys->device_list);
    vlc_mutex_unlock(&p_sys->device_list_lock);
    /* remove streams callbacks */
    CFIndex count = CFArrayGetCount(device_list_cpy);
    if (count > 0) {
        for (CFIndex x = 0; x < count; x++) {
            AudioDeviceID deviceId = 0;
            CFNumberRef cfn_device_id = CFArrayGetValueAtIndex(device_list_cpy, x);
            if (!cfn_device_id)
                continue;
            CFNumberGetValue(cfn_device_id, kCFNumberSInt32Type, &deviceId);
            if (!(deviceId & AOUT_VAR_SPDIF_FLAG)) {
                ManageAudioStreamsCallback(p_aout, deviceId, false);
            }
        }
    }
    CFRelease(device_list_cpy);
    CFRelease(p_sys->device_list);
    char *psz_device = aout_DeviceGet(p_aout);
    config_PutPsz(p_aout, "auhal-audio-device", psz_device);
    free(psz_device);
    vlc_mutex_destroy(&p_sys->selected_device_lock);
    vlc_mutex_destroy(&p_sys->device_list_lock);
    vlc_mutex_destroy(&p_sys->lock);
    vlc_cond_destroy(&p_sys->cond);
    free(p_sys);
}
