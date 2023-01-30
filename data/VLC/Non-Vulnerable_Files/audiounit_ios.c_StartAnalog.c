 */
static int StartAnalog(audio_output_t *p_aout, audio_sample_format_t *fmt)
{
    struct aout_sys_t           *p_sys = p_aout->sys;
    UInt32                      i_param_size = 0;
    AudioComponentDescription   desc;
    AURenderCallbackStruct      callback;
    OSStatus status;
    /* Lets go find our Component */
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    p_sys->au_component = AudioComponentFindNext(NULL, &desc);
    if (p_sys->au_component == NULL) {
        msg_Warn(p_aout, "we cannot find our audio component");
        return false;
    }
    status = AudioComponentInstanceNew(p_sys->au_component, &p_sys->au_unit);
    if (status != noErr) {
        msg_Warn(p_aout, "we cannot open our audio component (%i)", (int)status);
        return false;
    }
    UInt32 flag = 1;
    status = AudioUnitSetProperty(p_sys->au_unit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Output,
                                  0,
                                  &flag,
                                  sizeof(flag));
    if (status != noErr)
        msg_Warn(p_aout, "failed to set IO mode (%i)", (int)status);
    /* Get the current format */
    AudioStreamBasicDescription streamDescription;
    streamDescription.mSampleRate = fmt->i_rate;
    fmt->i_format = VLC_CODEC_FL32;
    fmt->i_physical_channels = fmt->i_original_channels = AOUT_CHANS_STEREO;
    streamDescription.mFormatID = kAudioFormatLinearPCM;
    streamDescription.mFormatFlags = kAudioFormatFlagsNativeFloatPacked; // FL32
    streamDescription.mChannelsPerFrame = aout_FormatNbChannels(fmt);
    streamDescription.mFramesPerPacket = 1;
    streamDescription.mBitsPerChannel = 32;
    streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
    streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;
    i_param_size = sizeof(streamDescription);
    p_sys->i_rate = fmt->i_rate;
    /* Set the desired format */
    i_param_size = sizeof(AudioStreamBasicDescription);
    status = AudioUnitSetProperty(p_sys->au_unit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  &streamDescription,
                                  i_param_size);
    if (status != noErr) {
        msg_Err(p_aout, "failed to set stream format (%i)", (int)status);
        return false;
    }
    msg_Dbg(p_aout, STREAM_FORMAT_MSG("we set the AU format: " , streamDescription));
    /* Retrieve actual format */
    status = AudioUnitGetProperty(p_sys->au_unit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  &streamDescription,
                                  &i_param_size);
    if (status != noErr)
        msg_Warn(p_aout, "failed to verify stream format (%i)", (int)status);
    msg_Dbg(p_aout, STREAM_FORMAT_MSG("the actual set AU format is " , streamDescription));
    /* Do the last VLC aout setups */
    aout_FormatPrepare(fmt);
    /* set the IOproc callback */
    callback.inputProc = (AURenderCallback) RenderCallback;
    callback.inputProcRefCon = p_aout;
    status = AudioUnitSetProperty(p_sys->au_unit,
                            kAudioUnitProperty_SetRenderCallback,
                            kAudioUnitScope_Input,
                            0, &callback, sizeof(callback));
    if (status != noErr) {
        msg_Err(p_aout, "render callback setup failed (%i)", (int)status);
        return false;
    }
    /* setup circular buffer */
    TPCircularBufferInit(&p_sys->circular_buffer, AUDIO_BUFFER_SIZE_IN_SECONDS * fmt->i_rate * fmt->i_bytes_per_frame);
    /* start audio session so playback continues if mute switch is on */
    AudioSessionInitialize (NULL,
                            kCFRunLoopCommonModes,
                            NULL,
                            NULL);
    /* Set audio session to mediaplayback */
    UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
    AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory),&sessionCategory);
    AudioSessionSetActive(true);
    /* AU init */
    status = AudioUnitInitialize(p_sys->au_unit);
    if (status != noErr) {
        msg_Err(p_aout, "failed to init AudioUnit (%i)", (int)status);
        return false;
    }
    /* start the unit */
    status = AudioOutputUnitStart(p_sys->au_unit);
    msg_Dbg(p_aout, "audio output unit started: %i", (int)status);
    return true;
}
