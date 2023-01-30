}
static int Start(audio_output_t *p_aout, audio_sample_format_t *restrict fmt)
{
    aout_sys_t *p_sys = p_aout->sys;
    OSStatus error = 0;
    // prepare the format description for our output
    AudioStreamBasicDescription streamDescription;
    streamDescription.mSampleRate = fmt->i_rate;
    streamDescription.mFormatID = kAudioFormatLinearPCM;
    streamDescription.mFormatFlags = kAudioFormatFlagsNativeFloatPacked; // FL32
    streamDescription.mFramesPerPacket = 1;
    streamDescription.mChannelsPerFrame = 2;
    streamDescription.mBitsPerChannel = 32;
    streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
    streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;
    // init new output instance
    error = AudioQueueNewOutput(&streamDescription,           // Format
                                 UnusedAudioQueueCallback,    // Unused Callback, which needs to be provided to have a proper instance
                                 NULL,                        // User data, passed to the callback
                                 NULL,                        // RunLoop
                                 kCFRunLoopCommonModes,       // RunLoop mode
                                 0,                           // Flags ; must be zero (per documentation)...
                                 &(p_sys->audioQueueRef));    // Output
    msg_Dbg(p_aout, "New AudioQueue instance created (status = %li)", error);
    if (error != noErr)
        return VLC_EGENERIC;
    fmt->i_format = VLC_CODEC_FL32;
    fmt->i_original_channels = fmt->i_physical_channels = AOUT_CHANS_STEREO;
    aout_FormatPrepare(fmt);
    p_aout->sys->i_rate = fmt->i_rate;
    // start queue
    error = AudioQueueStart(p_sys->audioQueueRef, NULL);
    msg_Dbg(p_aout, "Starting AudioQueue (status = %li)", error);
    // start timeline for synchro
    error = AudioQueueCreateTimeline(p_sys->audioQueueRef, &p_sys->timelineRef);
    msg_Dbg(p_aout, "AudioQueue Timeline started (status = %li)", error);
    if (error != noErr)
        return VLC_EGENERIC;
#if TARGET_OS_IPHONE
    // start audio session so playback continues if mute switch is on
    AudioSessionInitialize (NULL,
                            kCFRunLoopCommonModes,
                            NULL,
                            NULL);
	// Set audio session to mediaplayback
	UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory),&sessionCategory);
	AudioSessionSetActive(true);
#endif
    p_aout->sys->b_started = true;
    p_aout->time_get = TimeGet;
    p_aout->play = Play;
    p_aout->pause = Pause;
    p_aout->flush = Flush;
    return VLC_SUCCESS;
}
