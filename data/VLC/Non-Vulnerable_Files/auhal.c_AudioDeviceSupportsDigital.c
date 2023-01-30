 */
static int AudioDeviceSupportsDigital(audio_output_t *p_aout, AudioDeviceID i_dev_id)
{
    OSStatus                    err = noErr;
    UInt32                      i_param_size = 0;
    AudioStreamID               *p_streams = NULL;
    int                         i_streams = 0;
    bool                        b_return = false;
    /* Retrieve all the output streams */
    AudioObjectPropertyAddress streamsAddress = { kAudioDevicePropertyStreams, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
    err = AudioObjectGetPropertyDataSize(i_dev_id, &streamsAddress, 0, NULL, &i_param_size);
    if (err != noErr) {
        msg_Err(p_aout, "could not get number of streams [%4.4s] (%i)", (char *)&err, (int32_t)err);
        return false;
    }
    i_streams = i_param_size / sizeof(AudioStreamID);
    p_streams = (AudioStreamID *)malloc(i_param_size);
    if (p_streams == NULL)
        return VLC_ENOMEM;
    err = AudioObjectGetPropertyData(i_dev_id, &streamsAddress, 0, NULL, &i_param_size, p_streams);
    if (err != noErr) {
        msg_Err(p_aout, "could not get list of streams [%4.4s]", (char *)&err);
        free(p_streams);
        return false;
    }
    for (int i = 0; i < i_streams; i++) {
        if (AudioStreamSupportsDigital(p_aout, p_streams[i]))
            b_return = true;
    }
    free(p_streams);
    return b_return;
}
