#pragma mark helpers
static int ManageAudioStreamsCallback(audio_output_t *p_aout, AudioDeviceID i_dev_id, bool b_register)
{
    OSStatus                    err = noErr;
    UInt32                      i_param_size = 0;
    AudioStreamID               *p_streams = NULL;
    int                         i_streams = 0;
    /* Retrieve all the output streams */
    AudioObjectPropertyAddress streamsAddress = { kAudioDevicePropertyStreams, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
    err = AudioObjectGetPropertyDataSize(i_dev_id, &streamsAddress, 0, NULL, &i_param_size);
    if (err != noErr) {
        msg_Err(p_aout, "could not get number of streams for device id %i [%4.4s]", i_dev_id, (char *)&err);
        return VLC_EGENERIC;
    }
    i_streams = i_param_size / sizeof(AudioStreamID);
    p_streams = (AudioStreamID *)malloc(i_param_size);
    if (p_streams == NULL)
        return VLC_ENOMEM;
    err = AudioObjectGetPropertyData(i_dev_id, &streamsAddress, 0, NULL, &i_param_size, p_streams);
    if (err != noErr) {
        msg_Err(p_aout, "could not get list of streams [%4.4s]", (char *)&err);
        free(p_streams);
        return VLC_EGENERIC;
    }
    for (int i = 0; i < i_streams; i++) {
        /* get notified when physical formats change */
        AudioObjectPropertyAddress physicalFormatsAddress = { kAudioStreamPropertyAvailablePhysicalFormats, kAudioObjectPropertyScopeGlobal, 0 };
        if (b_register) {
            err = AudioObjectAddPropertyListener(p_streams[i], &physicalFormatsAddress, StreamsChangedListener, (void *)p_aout);
            if (err != noErr) {
                // nope just means that we already have a callback
                if (err == kAudioHardwareIllegalOperationError) {
                    msg_Warn(p_aout, "could not set audio stream formats property callback on stream id %i, callback already set? [%4.4s]", p_streams[i],
                             (char *)&err);
                } else {
                    msg_Err(p_aout, "could not set audio stream formats property callback on stream id %i [%4.4s]", p_streams[i],
                            (char *)&err);
                }
            }
        } else {  /* unregister callback */
            err = AudioObjectRemovePropertyListener(p_streams[i], &physicalFormatsAddress, StreamsChangedListener, (void *)p_aout);
            if (err != noErr)
                msg_Err(p_aout, "failed to remove audio device property streams callback [%4.4s]", (char *)&err);
        }
    }
    free(p_streams);
    return VLC_SUCCESS;
}
