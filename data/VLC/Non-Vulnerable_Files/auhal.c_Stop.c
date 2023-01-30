}
static void Stop(audio_output_t *p_aout)
{
    struct aout_sys_t   *p_sys = p_aout->sys;
    OSStatus            err = noErr;
    UInt32              i_param_size = 0;
    msg_Dbg(p_aout, "Stopping the auhal module");
    if (p_sys->au_unit) {
        verify_noerr(AudioOutputUnitStop(p_sys->au_unit));
        verify_noerr(AudioUnitUninitialize(p_sys->au_unit));
        verify_noerr(AudioComponentInstanceDispose(p_sys->au_unit));
    }
    if (p_sys->b_digital) {
        /* Stop device */
        err = AudioDeviceStop(p_sys->i_selected_dev,
                               p_sys->i_procID);
        if (err != noErr)
            msg_Err(p_aout, "Failed to stop audio device [%4.4s]", (char *)&err);
        /* Remove IOProc callback */
        err = AudioDeviceDestroyIOProcID(p_sys->i_selected_dev,
                                          p_sys->i_procID);
        if (err != noErr)
            msg_Err(p_aout, "Failed to destroy Process ID [%4.4s]", (char *)&err);
        if (p_sys->b_revert) {
            if (!AudioStreamChangeFormat(p_aout, p_sys->i_stream_id, p_sys->sfmt_revert)) {
                msg_Err(p_aout, "failed to revert stream format in close");
            }
        }
        if (p_sys->b_changed_mixing && p_sys->sfmt_revert.mFormatID != kAudioFormat60958AC3) {
            int b_mix;
            Boolean b_writeable = false;
            i_param_size = sizeof(int);
            /* Revert mixable to true if we are allowed to */
            AudioObjectPropertyAddress audioDeviceSupportsMixingAddress = { kAudioDevicePropertySupportsMixing , kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
            err = AudioObjectIsPropertySettable(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress, &b_writeable);
            err = AudioObjectGetPropertyData(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress, 0, NULL, &i_param_size, &b_mix);
            if (err == noErr && b_writeable) {
                msg_Dbg(p_aout, "mixable is: %d", b_mix);
                b_mix = 1;
                err = AudioObjectSetPropertyData(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress, 0, NULL, i_param_size, &b_mix);
            }
            if (err != noErr)
                msg_Err(p_aout, "failed to re-set mixmode [%4.4s]", (char *)&err);
        }
    }
    if (p_sys->i_hog_pid == getpid()) {
        p_sys->i_hog_pid = -1;
        i_param_size = sizeof(p_sys->i_hog_pid);
        AudioObjectPropertyAddress audioDeviceHogModeAddress = { kAudioDevicePropertyHogMode,
            kAudioDevicePropertyScopeOutput,
            kAudioObjectPropertyElementMaster };
        /*
         * HACK: On 10.6, auhal will trigger the streams changed callback when calling below line,
         * directly in the same thread. This call needs to be ignored to avoid endless restarting.
         */
        p_sys->b_ignore_streams_changed_callback = true;
        err = AudioObjectSetPropertyData(p_sys->i_selected_dev, &audioDeviceHogModeAddress, 0, NULL, i_param_size, &p_sys->i_hog_pid);
        p_sys->b_ignore_streams_changed_callback = false;
        if (err != noErr)
            msg_Err(p_aout, "Failed to release hogmode [%4.4s]", (char *)&err);
    }
    /* remove audio device alive callback */
    AudioObjectPropertyAddress deviceAliveAddress = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    err = AudioObjectRemovePropertyListener(p_sys->i_selected_dev, &deviceAliveAddress, DeviceAliveListener, (void *)p_aout);
    if (err != noErr) {
        /* Be tolerant, only give a warning here */
        msg_Warn(p_aout, "failed to remove audio device life checker [%4.4s]", (char *)&err);
    }
    p_sys->i_bytes_per_sample = 0;
    p_sys->b_digital = false;
    /* clean-up circular buffer */
    TPCircularBufferCleanup(&p_sys->circular_buffer);
}
