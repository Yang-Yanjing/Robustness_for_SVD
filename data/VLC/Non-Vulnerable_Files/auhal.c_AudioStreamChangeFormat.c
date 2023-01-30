 */
static int AudioStreamChangeFormat(audio_output_t *p_aout, AudioStreamID i_stream_id, AudioStreamBasicDescription change_format)
{
    OSStatus err = noErr;
    int retValue = false;
    AudioObjectPropertyAddress physicalFormatAddress = { kAudioStreamPropertyPhysicalFormat, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    struct { vlc_mutex_t lock; vlc_cond_t cond; } w;
    msg_Dbg(p_aout, STREAM_FORMAT_MSG("setting stream format: ", change_format));
    /* Condition because SetProperty is asynchronious */
    vlc_cond_init(&w.cond);
    vlc_mutex_init(&w.lock);
    vlc_mutex_lock(&w.lock);
    /* Install the callback */
    err = AudioObjectAddPropertyListener(i_stream_id, &physicalFormatAddress, StreamListener, (void *)&w);
    if (err != noErr) {
        msg_Err(p_aout, "AudioObjectAddPropertyListener for kAudioStreamPropertyPhysicalFormat failed [%4.4s]", (char *)&err);
        retValue = false;
        goto out;
    }
    /* change the format */
    err = AudioObjectSetPropertyData(i_stream_id, &physicalFormatAddress, 0, NULL, sizeof(AudioStreamBasicDescription),
                                     &change_format);
    if (err != noErr) {
        msg_Err(p_aout, "could not set the stream format [%4.4s]", (char *)&err);
        retValue = false;
        goto out;
    }
    /* The AudioStreamSetProperty is not only asynchronious (requiring the locks)
     * it is also not atomic in its behaviour.
     * Therefore we check 9 times before we really give up.
     */
    AudioStreamBasicDescription actual_format;
    UInt32 i_param_size = sizeof(AudioStreamBasicDescription);
    for (int i = 0; i < 9; i++) {
        /* Callback is not always invoked. So first check if format is already set. */
        if (i > 0) {
            mtime_t timeout = mdate() + 500000;
            if (vlc_cond_timedwait(&w.cond, &w.lock, timeout))
                msg_Dbg(p_aout, "reached timeout");
        }
        err = AudioObjectGetPropertyData(i_stream_id, &physicalFormatAddress, 0, NULL, &i_param_size, &actual_format);
        msg_Dbg(p_aout, STREAM_FORMAT_MSG("actual format in use: ", actual_format));
        if (actual_format.mSampleRate == change_format.mSampleRate &&
            actual_format.mFormatID == change_format.mFormatID &&
            actual_format.mFramesPerPacket == change_format.mFramesPerPacket) {
            /* The right format is now active */
            retValue = true;
            break;
        }
        /* We need to check again */
    }
out:
    vlc_mutex_unlock(&w.lock);
    /* Removing the property listener */
    err = AudioObjectRemovePropertyListener(i_stream_id, &physicalFormatAddress, StreamListener, (void *)&w);
    if (err != noErr) {
        msg_Err(p_aout, "AudioStreamRemovePropertyListener failed [%4.4s]", (char *)&err);
        retValue = false;
    }
    vlc_mutex_destroy(&w.lock);
    vlc_cond_destroy(&w.cond);
    return retValue;
}
