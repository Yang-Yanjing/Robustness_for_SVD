}
static int Start(audio_output_t *p_aout, audio_sample_format_t *restrict fmt)
{
    OSStatus                err = noErr;
    UInt32                  i_param_size = 0;
    struct aout_sys_t       *p_sys = NULL;
    /* Use int here, to match kAudioDevicePropertyDeviceIsAlive
     * property size */
    int                     b_alive = false;
    bool                    b_start_digital = false;
    p_sys = p_aout->sys;
    p_sys->b_digital = false;
    p_sys->au_component = NULL;
    p_sys->au_unit = NULL;
    p_sys->i_hog_pid = -1;
    p_sys->i_stream_index = -1;
    p_sys->b_revert = false;
    p_sys->b_changed_mixing = false;
    p_sys->i_bytes_per_sample = 0;
    p_sys->b_paused = false;
    p_sys->i_device_latency = 0;
    vlc_mutex_lock(&p_sys->selected_device_lock);
    p_sys->i_selected_dev = p_sys->i_new_selected_dev;
    aout_FormatPrint(p_aout, "VLC is looking for:", fmt);
    msg_Dbg(p_aout, "attempting to use device %i", p_sys->i_selected_dev);
    AudioObjectPropertyAddress audioDeviceAliveAddress = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    if (p_sys->i_selected_dev > 0) {
        /* Check if device is in devices list. Only checking for kAudioDevicePropertyDeviceIsAlive is not sufficient,
         * as a former airplay device might be already gone, but the device number might be still valid. Core Audio
         * even says that this device would be alive. Don't ask why, its Core Audio. */
        CFIndex count = CFArrayGetCount(p_sys->device_list);
        CFNumberRef deviceNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &p_sys->i_selected_dev);
        if (CFArrayContainsValue(p_sys->device_list, CFRangeMake(0, count), deviceNumber)) {
            /* Check if the desired device is alive and usable */
            i_param_size = sizeof(b_alive);
            err = AudioObjectGetPropertyData(p_sys->i_selected_dev, &audioDeviceAliveAddress, 0, NULL, &i_param_size, &b_alive);
            if (err != noErr) {
                /* Be tolerant, only give a warning here */
                msg_Warn(p_aout, "could not check whether device [0x%x] is alive [%4.4s]",
                         (unsigned int)p_sys->i_selected_dev, (char *)&err);
                b_alive = false;
            }
            if (!b_alive)
                msg_Warn(p_aout, "selected audio device is not alive, switching to default device");
        } else {
            msg_Warn(p_aout, "device id %i not found in the current devices list, fallback to default device", p_sys->i_selected_dev);
        }
        CFRelease(deviceNumber);
    }
    p_sys->b_selected_dev_is_default = false;
    if (!b_alive || p_sys->i_selected_dev == 0) {
        p_sys->b_selected_dev_is_default = true;
        AudioObjectID defaultDeviceID = 0;
        UInt32 propertySize = sizeof(AudioObjectID);
        AudioObjectPropertyAddress defaultDeviceAddress = { kAudioHardwarePropertyDefaultOutputDevice, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
        propertySize = sizeof(AudioObjectID);
        err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultDeviceAddress, 0, NULL, &propertySize, &defaultDeviceID);
        if (err != noErr) {
            msg_Err(p_aout, "could not get default audio device [%4.4s]", (char *)&err);
            vlc_mutex_unlock(&p_sys->selected_device_lock);
            goto error;
        }
        else
            msg_Dbg(p_aout, "using default audio device %i", defaultDeviceID);
        p_sys->i_selected_dev = defaultDeviceID;
        p_sys->b_selected_dev_is_digital = var_InheritBool(p_aout, "spdif");
    }
    vlc_mutex_unlock(&p_sys->selected_device_lock);
    // recheck if device still supports digital
    b_start_digital = p_sys->b_selected_dev_is_digital;
    if(!AudioDeviceSupportsDigital(p_aout, p_sys->i_selected_dev))
        b_start_digital = false;
    if (b_start_digital)
        msg_Dbg(p_aout, "Using audio device for digital output");
    else
        msg_Dbg(p_aout, "Audio device supports PCM mode only");
    /* add a callback to see if the device dies later on */
    err = AudioObjectAddPropertyListener(p_sys->i_selected_dev, &audioDeviceAliveAddress, DeviceAliveListener, (void *)p_aout);
    if (err != noErr) {
        /* Be tolerant, only give a warning here */
        msg_Warn(p_aout, "could not set alive check callback on device [0x%x] [%4.4s]",
                 (unsigned int)p_sys->i_selected_dev, (char *)&err);
    }
    AudioObjectPropertyAddress audioDeviceHogModeAddress = { kAudioDevicePropertyHogMode,
                                  kAudioDevicePropertyScopeOutput,
                                  kAudioObjectPropertyElementMaster };
    i_param_size = sizeof(p_sys->i_hog_pid);
    err = AudioObjectGetPropertyData(p_sys->i_selected_dev, &audioDeviceHogModeAddress, 0, NULL, &i_param_size, &p_sys->i_hog_pid);
    if (err != noErr) {
        /* This is not a fatal error. Some drivers simply don't support this property */
        msg_Warn(p_aout, "could not check whether device is hogged [%4.4s]",
                 (char *)&err);
        p_sys->i_hog_pid = -1;
    }
    if (p_sys->i_hog_pid != -1 && p_sys->i_hog_pid != getpid()) {
        msg_Err(p_aout, "Selected audio device is exclusively in use by another program.");
        dialog_Fatal(p_aout, _("Audio output failed"), "%s",
                        _("The selected audio output device is exclusively in "
                          "use by another program."));
        goto error;
    }
    /* get device latency */
    AudioObjectPropertyAddress latencyAddress = { kAudioDevicePropertyLatency, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
    i_param_size = sizeof(p_sys->i_device_latency);
    err = AudioObjectGetPropertyData(p_sys->i_selected_dev,
                                     &latencyAddress,
                                     0,
                                     NULL,
                                     &i_param_size,
                                     &p_sys->i_device_latency);
    if (err != noErr) {
        msg_Warn(p_aout, "Cannot get device latency [%4.4s]",
                 (char *)&err);
    }
    float f_latency_in_sec = (float)p_sys->i_device_latency / (float)fmt->i_rate;
    msg_Dbg(p_aout, "Current device has a latency of %u frames (%f sec)", p_sys->i_device_latency, f_latency_in_sec);
    // Ignore long Airplay latency as this is not correctly working yet
    if (f_latency_in_sec > 0.5f) {
        msg_Info(p_aout, "Ignore high latency as it causes problems currently.");
        p_sys->i_device_latency = 0;
    }
    bool b_success = false;
    /* Check for Digital mode or Analog output mode */
    if (AOUT_FMT_SPDIF (fmt) && b_start_digital) {
        if (StartSPDIF (p_aout, fmt)) {
            msg_Dbg(p_aout, "digital output successfully opened");
            b_success = true;
        }
    } else {
        if (StartAnalog(p_aout, fmt)) {
            msg_Dbg(p_aout, "analog output successfully opened");
            b_success = true;
        }
    }
    if (b_success) {
        p_aout->play = Play;
        p_aout->flush = Flush;
        p_aout->time_get = TimeGet;
        p_aout->pause = Pause;
        return VLC_SUCCESS;
    }
error:
    /* If we reach this, this aout has failed */
    msg_Err(p_aout, "opening auhal output failed");
    return VLC_EGENERIC;
}
