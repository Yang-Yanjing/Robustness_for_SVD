 */
static int StartAnalog(audio_output_t *p_aout, audio_sample_format_t *fmt)
{
    struct aout_sys_t           *p_sys = p_aout->sys;
    OSStatus                    err = noErr;
    UInt32                      i_param_size = 0;
    int                         i_original;
    AudioComponentDescription   desc;
    AudioStreamBasicDescription DeviceFormat;
    AudioChannelLayout          *layout;
    AURenderCallbackStruct      input;
    p_aout->sys->chans_to_reorder = 0;
    SInt32 currentMinorSystemVersion;
    if(Gestalt(gestaltSystemVersionMinor, &currentMinorSystemVersion) != noErr)
        msg_Err(p_aout, "failed to check OSX version");
    /* Lets go find our Component */
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_HALOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    p_sys->au_component = AudioComponentFindNext(NULL, &desc);
    if (p_sys->au_component == NULL) {
        msg_Err(p_aout, "cannot find any HAL component, PCM output failed");
        return false;
    }
    err = AudioComponentInstanceNew(p_sys->au_component, &p_sys->au_unit);
    if (err != noErr) {
        msg_Err(p_aout, "cannot open HAL component, PCM output failed [%4.4s]", (char *)&err);
        return false;
    }
    /* Set the device we will use for this output unit */
    err = AudioUnitSetProperty(p_sys->au_unit,
                         kAudioOutputUnitProperty_CurrentDevice,
                         kAudioUnitScope_Global,
                         0,
                         &p_sys->i_selected_dev,
                         sizeof(AudioObjectID));
    if (err != noErr) {
        msg_Err(p_aout, "cannot select audio output device, PCM output failed [%4.4s]", (char *)&err);
        return false;
    }
    /* Get the current format */
    i_param_size = sizeof(AudioStreamBasicDescription);
    err = AudioUnitGetProperty(p_sys->au_unit,
                                   kAudioUnitProperty_StreamFormat,
                                   kAudioUnitScope_Output,
                                   0,
                                   &DeviceFormat,
                                   &i_param_size);
    if (err != noErr) {
        msg_Err(p_aout, "failed to detect supported stream formats [%4.4s]", (char *)&err);
        return false;
    } else
        msg_Dbg(p_aout, STREAM_FORMAT_MSG("current format is: ", DeviceFormat));
    /* Get the channel layout of the device side of the unit (vlc -> unit -> device) */
    err = AudioUnitGetPropertyInfo(p_sys->au_unit,
                                   kAudioDevicePropertyPreferredChannelLayout,
                                   kAudioUnitScope_Output,
                                   0,
                                   &i_param_size,
                                   NULL);
    if (err == noErr) {
        layout = (AudioChannelLayout *)malloc(i_param_size);
        verify_noerr(AudioUnitGetProperty(p_sys->au_unit,
                                       kAudioDevicePropertyPreferredChannelLayout,
                                       kAudioUnitScope_Output,
                                       0,
                                       layout,
                                       &i_param_size));
        /* We need to "fill out" the ChannelLayout, because there are multiple ways that it can be set */
        if (layout->mChannelLayoutTag == kAudioChannelLayoutTag_UseChannelBitmap) {
            /* bitmap defined channellayout */
            verify_noerr(AudioFormatGetProperty(kAudioFormatProperty_ChannelLayoutForBitmap,
                                    sizeof(UInt32), &layout->mChannelBitmap,
                                    &i_param_size,
                                    layout));
        } else if (layout->mChannelLayoutTag != kAudioChannelLayoutTag_UseChannelDescriptions)
        {
            /* layouttags defined channellayout */
            verify_noerr(AudioFormatGetProperty(kAudioFormatProperty_ChannelLayoutForTag,
                                    sizeof(AudioChannelLayoutTag), &layout->mChannelLayoutTag,
                                    &i_param_size,
                                    layout));
        }
        msg_Dbg(p_aout, "layout of AUHAL has %i channels" , layout->mNumberChannelDescriptions);
        if (layout->mNumberChannelDescriptions == 0) {
            msg_Err(p_aout, "insufficient number of output channels");
            free(layout);
            return false;
        }
        /* Initialize the VLC core channel count */
        fmt->i_physical_channels = 0;
        i_original = fmt->i_original_channels & AOUT_CHAN_PHYSMASK;
        if (i_original == AOUT_CHAN_CENTER || layout->mNumberChannelDescriptions < 2) {
            /* We only need Mono or cannot output more than 1 channel */
            fmt->i_physical_channels = AOUT_CHAN_CENTER;
        } else if (i_original == (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT) || layout->mNumberChannelDescriptions < 3) {
            /* We only need Stereo or cannot output more than 2 channels */
            fmt->i_physical_channels = AOUT_CHANS_STEREO;
        } else {
            // maps auhal channels to vlc ones
            static const unsigned i_auhal_channel_mapping[] = {
                [kAudioChannelLabel_Left]           = AOUT_CHAN_LEFT,
                [kAudioChannelLabel_Right]          = AOUT_CHAN_RIGHT,
                [kAudioChannelLabel_Center]         = AOUT_CHAN_CENTER,
                [kAudioChannelLabel_LFEScreen]      = AOUT_CHAN_LFE,
                [kAudioChannelLabel_LeftSurround]   = AOUT_CHAN_REARLEFT,
                [kAudioChannelLabel_RightSurround]  = AOUT_CHAN_REARRIGHT,
                [kAudioChannelLabel_RearSurroundLeft]  = AOUT_CHAN_MIDDLELEFT, // needs to be swapped with rear
                [kAudioChannelLabel_RearSurroundRight] = AOUT_CHAN_MIDDLERIGHT,// needs to be swapped with rear
                [kAudioChannelLabel_CenterSurround] = AOUT_CHAN_REARCENTER
            };
            /* We want more than stereo and we can do that */
            for (unsigned int i = 0; i < layout->mNumberChannelDescriptions; i++) {
#ifndef NDEBUG
                msg_Dbg(p_aout, "this is channel: %d", (int)layout->mChannelDescriptions[i].mChannelLabel);
#endif
                AudioChannelLabel chan = layout->mChannelDescriptions[i].mChannelLabel;
                if(chan < sizeof(i_auhal_channel_mapping) / sizeof(i_auhal_channel_mapping[0])
                   && i_auhal_channel_mapping[chan] > 0) {
                    fmt->i_physical_channels |= i_auhal_channel_mapping[chan];
                } else {
                    msg_Dbg(p_aout, "found nonrecognized channel %d at index %d", chan, i);
                }
            }
            if (fmt->i_physical_channels == 0) {
                fmt->i_physical_channels = AOUT_CHANS_STEREO;
                msg_Err(p_aout, "You should configure your speaker layout with Audio Midi Setup in /Applications/Utilities. VLC will output Stereo only.");
                dialog_Fatal(p_aout, _("Audio device is not configured"), "%s",
                                _("You should configure your speaker layout with "
                                  "\"Audio Midi Setup\" in /Applications/"
                                  "Utilities. VLC will output Stereo only."));
            }
        }
        free(layout);
    } else {
        msg_Warn(p_aout, "device driver does not support kAudioDevicePropertyPreferredChannelLayout - using stereo fallback [%4.4s]", (char *)&err);
        fmt->i_physical_channels = AOUT_CHANS_STEREO;
    }
    fmt->i_original_channels = fmt->i_physical_channels;
    msg_Dbg(p_aout, "selected %d physical channels for device output", aout_FormatNbChannels(fmt));
    msg_Dbg(p_aout, "VLC will output: %s", aout_FormatPrintChannels(fmt));
    /* Now we set the INPUT layout of the AU */
    AudioChannelLayout input_layout;
    memset (&input_layout, 0, sizeof(input_layout));
    uint32_t chans_out[AOUT_CHAN_MAX];
    /* Some channel abbreviations used below:
     * L - left
     * R - right
     * C - center
     * Ls - left surround
     * Rs - right surround
     * Cs - center surround
     * Rls - rear left surround
     * Rrs - rear right surround
     * Lw - left wide
     * Rw - right wide
     * Lsd - left surround direct
     * Rsd - right surround direct
     * Lc - left center
     * Rc - right center
     * Ts - top surround
     * Vhl - vertical height left
     * Vhc - vertical height center
     * Vhr - vertical height right
     * Lt - left matrix total. for matrix encoded stereo.
     * Rt - right matrix total. for matrix encoded stereo. */
    switch(aout_FormatNbChannels(fmt)) {
        case 1:
            input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_Mono;
            break;
        case 2:
            input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_Stereo;
            break;
        case 3:
            if (fmt->i_physical_channels & AOUT_CHAN_CENTER)
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_7; // L R C
            else if (fmt->i_physical_channels & AOUT_CHAN_LFE)
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_4; // L R LFE
            break;
        case 4:
            if (fmt->i_physical_channels & (AOUT_CHAN_CENTER | AOUT_CHAN_LFE))
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_10; // L R C LFE
            else if (fmt->i_physical_channels & (AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT))
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_3; // L R Ls Rs
            else if (fmt->i_physical_channels & (AOUT_CHAN_CENTER | AOUT_CHAN_REARCENTER))
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_3; // L R C Cs
            break;
        case 5:
            if (fmt->i_physical_channels & (AOUT_CHAN_CENTER))
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_19; // L R Ls Rs C
            else if (fmt->i_physical_channels & (AOUT_CHAN_LFE))
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_18; // L R Ls Rs LFE
            break;
        case 6:
            if (fmt->i_physical_channels & (AOUT_CHAN_LFE)) {
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DVD_20; // L R Ls Rs C LFE
                chans_out[0] = AOUT_CHAN_LEFT;
                chans_out[1] = AOUT_CHAN_RIGHT;
                chans_out[2] = AOUT_CHAN_REARLEFT;
                chans_out[3] = AOUT_CHAN_REARRIGHT;
                chans_out[4] = AOUT_CHAN_CENTER;
                chans_out[5] = AOUT_CHAN_LFE;
                p_aout->sys->chans_to_reorder = aout_CheckChannelReorder(NULL, chans_out, fmt->i_physical_channels, p_aout->sys->chan_table);
                if (p_aout->sys->chans_to_reorder)
                    msg_Dbg(p_aout, "channel reordering needed for 5.1 output");
            } else {
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_AudioUnit_6_0; // L R Ls Rs C Cs
                chans_out[0] = AOUT_CHAN_LEFT;
                chans_out[1] = AOUT_CHAN_RIGHT;
                chans_out[2] = AOUT_CHAN_REARLEFT;
                chans_out[3] = AOUT_CHAN_REARRIGHT;
                chans_out[4] = AOUT_CHAN_CENTER;
                chans_out[5] = AOUT_CHAN_REARCENTER;
                p_aout->sys->chans_to_reorder = aout_CheckChannelReorder(NULL, chans_out, fmt->i_physical_channels, p_aout->sys->chan_table);
                if (p_aout->sys->chans_to_reorder)
                    msg_Dbg(p_aout, "channel reordering needed for 6.0 output");
            }
            break;
        case 7:
            input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_MPEG_6_1_A; // L R C LFE Ls Rs Cs
            chans_out[0] = AOUT_CHAN_LEFT;
            chans_out[1] = AOUT_CHAN_RIGHT;
            chans_out[2] = AOUT_CHAN_CENTER;
            chans_out[3] = AOUT_CHAN_LFE;
            chans_out[4] = AOUT_CHAN_REARLEFT;
            chans_out[5] = AOUT_CHAN_REARRIGHT;
            chans_out[6] = AOUT_CHAN_REARCENTER;
            p_aout->sys->chans_to_reorder = aout_CheckChannelReorder(NULL, chans_out, fmt->i_physical_channels, p_aout->sys->chan_table);
            if (p_aout->sys->chans_to_reorder)
                msg_Dbg(p_aout, "channel reordering needed for 6.1 output");
            break;
        case 8:
            if (fmt->i_physical_channels & (AOUT_CHAN_LFE) || currentMinorSystemVersion < 7) {
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_MPEG_7_1_A; // L R C LFE Ls Rs Lc Rc
                chans_out[0] = AOUT_CHAN_LEFT;
                chans_out[1] = AOUT_CHAN_RIGHT;
                chans_out[2] = AOUT_CHAN_CENTER;
                chans_out[3] = AOUT_CHAN_LFE;
                chans_out[4] = AOUT_CHAN_MIDDLELEFT;
                chans_out[5] = AOUT_CHAN_MIDDLERIGHT;
                chans_out[6] = AOUT_CHAN_REARLEFT;
                chans_out[7] = AOUT_CHAN_REARRIGHT;
                if (!(fmt->i_physical_channels & (AOUT_CHAN_LFE)))
                    msg_Warn(p_aout, "8.0 audio output not supported on OS X 10.%i, layout will be incorrect", currentMinorSystemVersion);
            }
#ifdef MAC_OS_X_VERSION_10_7
            else {
                input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DTS_8_0_B; // Lc C Rc L R Ls Cs Rs
                chans_out[0] = AOUT_CHAN_MIDDLELEFT;
                chans_out[1] = AOUT_CHAN_CENTER;
                chans_out[2] = AOUT_CHAN_MIDDLERIGHT;
                chans_out[3] = AOUT_CHAN_LEFT;
                chans_out[4] = AOUT_CHAN_RIGHT;
                chans_out[5] = AOUT_CHAN_REARLEFT;
                chans_out[6] = AOUT_CHAN_REARCENTER;
                chans_out[7] = AOUT_CHAN_REARRIGHT;
            }
#endif
            p_aout->sys->chans_to_reorder = aout_CheckChannelReorder(NULL, chans_out, fmt->i_physical_channels, p_aout->sys->chan_table);
            if (p_aout->sys->chans_to_reorder)
                msg_Dbg(p_aout, "channel reordering needed for 7.1 / 8.0 output");
            break;
        case 9:
            if (currentMinorSystemVersion < 7) {
                msg_Warn(p_aout, "8.1 audio output not supported on OS X 10.%i", currentMinorSystemVersion);
                break;
            }
#ifdef MAC_OS_X_VERSION_10_7
            input_layout.mChannelLayoutTag = kAudioChannelLayoutTag_DTS_8_1_B; // Lc C Rc L R Ls Cs Rs LFE
            chans_out[0] = AOUT_CHAN_MIDDLELEFT;
            chans_out[1] = AOUT_CHAN_CENTER;
            chans_out[2] = AOUT_CHAN_MIDDLERIGHT;
            chans_out[3] = AOUT_CHAN_LEFT;
            chans_out[4] = AOUT_CHAN_RIGHT;
            chans_out[5] = AOUT_CHAN_REARLEFT;
            chans_out[6] = AOUT_CHAN_REARCENTER;
            chans_out[7] = AOUT_CHAN_REARRIGHT;
            chans_out[8] = AOUT_CHAN_LFE;
            p_aout->sys->chans_to_reorder = aout_CheckChannelReorder(NULL, chans_out, fmt->i_physical_channels, p_aout->sys->chan_table);
            if (p_aout->sys->chans_to_reorder)
                msg_Dbg(p_aout, "channel reordering needed for 8.1 output");
#endif
            break;
    }
    /* Set up the format to be used */
    DeviceFormat.mSampleRate = fmt->i_rate;
    DeviceFormat.mFormatID = kAudioFormatLinearPCM;
    p_sys->i_rate = fmt->i_rate;
    /* We use float 32 since this is VLC's endorsed format */
    fmt->i_format = VLC_CODEC_FL32;
    DeviceFormat.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
    DeviceFormat.mBitsPerChannel = 32;
    DeviceFormat.mChannelsPerFrame = aout_FormatNbChannels(fmt);
    /* Calculate framesizes and stuff */
    DeviceFormat.mFramesPerPacket = 1;
    DeviceFormat.mBytesPerFrame = DeviceFormat.mBitsPerChannel * DeviceFormat.mChannelsPerFrame / 8;
    DeviceFormat.mBytesPerPacket = DeviceFormat.mBytesPerFrame * DeviceFormat.mFramesPerPacket;
    /* Set the desired format */
    i_param_size = sizeof(AudioStreamBasicDescription);
    verify_noerr(AudioUnitSetProperty(p_sys->au_unit,
                                   kAudioUnitProperty_StreamFormat,
                                   kAudioUnitScope_Input,
                                   0,
                                   &DeviceFormat,
                                   i_param_size));
    msg_Dbg(p_aout, STREAM_FORMAT_MSG("we set the AU format: " , DeviceFormat));
    /* Retrieve actual format */
    verify_noerr(AudioUnitGetProperty(p_sys->au_unit,
                                   kAudioUnitProperty_StreamFormat,
                                   kAudioUnitScope_Input,
                                   0,
                                   &DeviceFormat,
                                   &i_param_size));
    msg_Dbg(p_aout, STREAM_FORMAT_MSG("the actual set AU format is " , DeviceFormat));
    /* Do the last VLC aout setups */
    aout_FormatPrepare(fmt);
    /* set the IOproc callback */
    input.inputProc = (AURenderCallback) RenderCallbackAnalog;
    input.inputProcRefCon = p_aout;
    verify_noerr(AudioUnitSetProperty(p_sys->au_unit,
                            kAudioUnitProperty_SetRenderCallback,
                            kAudioUnitScope_Input,
                            0, &input, sizeof(input)));
    /* Set the input_layout as the layout VLC will use to feed the AU unit */
    verify_noerr(AudioUnitSetProperty(p_sys->au_unit,
                            kAudioUnitProperty_AudioChannelLayout,
                            kAudioUnitScope_Input, /* yes, it must be the INPUT scope */
                            0, &input_layout, sizeof(input_layout)));
    /* AU initiliaze */
    verify_noerr(AudioUnitInitialize(p_sys->au_unit));
    /* setup circular buffer */
    TPCircularBufferInit(&p_sys->circular_buffer, AUDIO_BUFFER_SIZE_IN_SECONDS *
                         fmt->i_rate * fmt->i_bytes_per_frame);
    verify_noerr(AudioOutputUnitStart(p_sys->au_unit));
    /* Set volume for output unit */
    VolumeSet(p_aout, p_sys->f_volume);
    MuteSet(p_aout, p_sys->b_mute);
    return true;
}
