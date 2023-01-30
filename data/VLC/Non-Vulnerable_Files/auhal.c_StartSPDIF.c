 */
static int StartSPDIF(audio_output_t * p_aout, audio_sample_format_t *fmt)
{
    struct aout_sys_t       *p_sys = p_aout->sys;
    OSStatus                err = noErr;
    UInt32                  i_param_size = 0, b_mix = 0;
    Boolean                 b_writeable = false;
    AudioStreamID           *p_streams = NULL;
    unsigned                i_streams = 0;
    AudioStreamBasicDescription desired_stream_format;
    memset(&desired_stream_format, 0, sizeof(desired_stream_format));
    /* Start doing the SPDIF setup proces */
    p_sys->b_digital = true;
    /* Hog the device */
    AudioObjectPropertyAddress audioDeviceHogModeAddress = { kAudioDevicePropertyHogMode, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
    i_param_size = sizeof(p_sys->i_hog_pid);
    p_sys->i_hog_pid = getpid() ;
    /*
     * HACK: On 10.6, auhal will trigger the streams changed callback when calling below line,
     * directly in the same thread. This call needs to be ignored to avoid endless restarting.
     */
    p_sys->b_ignore_streams_changed_callback = true;
    err = AudioObjectSetPropertyData(p_sys->i_selected_dev, &audioDeviceHogModeAddress, 0, NULL, i_param_size, &p_sys->i_hog_pid);
    p_sys->b_ignore_streams_changed_callback = false;
    if (err != noErr) {
        msg_Err(p_aout, "failed to set hogmode [%4.4s]", (char *)&err);
        return false;
    }
    AudioObjectPropertyAddress audioDeviceSupportsMixingAddress = { kAudioDevicePropertySupportsMixing , kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    if (AudioObjectHasProperty(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress)) {
        /* Set mixable to false if we are allowed to */
        err = AudioObjectIsPropertySettable(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress, &b_writeable);
        err = AudioObjectGetPropertyDataSize(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress, 0, NULL, &i_param_size);
        err = AudioObjectGetPropertyData(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress, 0, NULL, &i_param_size, &b_mix);
        if (err == noErr && b_writeable) {
            b_mix = 0;
            err = AudioObjectSetPropertyData(p_sys->i_selected_dev, &audioDeviceSupportsMixingAddress, 0, NULL, i_param_size, &b_mix);
            p_sys->b_changed_mixing = true;
        }
        if (err != noErr) {
            msg_Err(p_aout, "failed to set mixmode [%4.4s]", (char *)&err);
            return false;
        }
    }
    /* Get a list of all the streams on this device */
    AudioObjectPropertyAddress streamsAddress = { kAudioDevicePropertyStreams, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
    err = AudioObjectGetPropertyDataSize(p_sys->i_selected_dev, &streamsAddress, 0, NULL, &i_param_size);
    if (err != noErr) {
        msg_Err(p_aout, "could not get size of stream description packet [%4.4s]", (char *)&err);
        return false;
    }
    i_streams = i_param_size / sizeof(AudioStreamID);
    p_streams = (AudioStreamID *)malloc(i_param_size);
    if (p_streams == NULL)
        return false;
    err = AudioObjectGetPropertyData(p_sys->i_selected_dev, &streamsAddress, 0, NULL, &i_param_size, p_streams);
    if (err != noErr) {
        msg_Err(p_aout, "could not fetch stream descriptions [%4.4s]", (char *)&err);
        free(p_streams);
        return false;
    }
    AudioObjectPropertyAddress physicalFormatsAddress = { kAudioStreamPropertyAvailablePhysicalFormats, kAudioObjectPropertyScopeGlobal, 0 };
    for (unsigned i = 0; i < i_streams && p_sys->i_stream_index < 0 ; i++) {
        /* Find a stream with a cac3 stream */
        AudioStreamRangedDescription *p_format_list = NULL;
        int                          i_formats = 0;
        bool                         b_digital = false;
        /* Retrieve all the stream formats supported by each output stream */
        err = AudioObjectGetPropertyDataSize(p_streams[i], &physicalFormatsAddress, 0, NULL, &i_param_size);
        if (err != noErr) {
            msg_Err(p_aout, "could not get number of streamformats: [%4.4s] (%i)", (char *)&err, (int32_t)err);
            continue;
        }
        i_formats = i_param_size / sizeof(AudioStreamRangedDescription);
        p_format_list = (AudioStreamRangedDescription *)malloc(i_param_size);
        if (p_format_list == NULL)
            continue;
        err = AudioObjectGetPropertyData(p_streams[i], &physicalFormatsAddress, 0, NULL, &i_param_size, p_format_list);
        if (err != noErr) {
            msg_Err(p_aout, "could not get the list of streamformats: [%4.4s]", (char *)&err);
            free(p_format_list);
            continue;
        }
        /* Check if one of the supported formats is a digital format */
        for (int j = 0; j < i_formats; j++) {
            if (p_format_list[j].mFormat.mFormatID == 'IAC3' ||
               p_format_list[j].mFormat.mFormatID == 'iac3' ||
               p_format_list[j].mFormat.mFormatID == kAudioFormat60958AC3 ||
               p_format_list[j].mFormat.mFormatID == kAudioFormatAC3) {
                b_digital = true;
                break;
            }
        }
        if (b_digital) {
            /* if this stream supports a digital (cac3) format, then go set it. */
            int i_requested_rate_format = -1;
            int i_current_rate_format = -1;
            int i_backup_rate_format = -1;
            if (!p_sys->b_revert) {
                AudioObjectPropertyAddress currentPhysicalFormatAddress = { kAudioStreamPropertyPhysicalFormat, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
                /* Retrieve the original format of this stream first if not done so already */
                AudioStreamBasicDescription current_streamformat;
                i_param_size = sizeof(current_streamformat);
                err = AudioObjectGetPropertyData(p_streams[i], &currentPhysicalFormatAddress, 0, NULL, &i_param_size, &current_streamformat);
                if (err != noErr) {
                    msg_Err(p_aout, "could not retrieve the original streamformat [%4.4s]", (char *)&err);
                    continue;
                }
                /* 
                 * Only the first found format id is accepted. In case of another id later on, we still use the
                 * already saved one. This can happen if the user plugs in a spdif cable while a stream is already
                 * playing. Then, auhal already misleadingly reports an ac3 format here whereas the original format
                 * should be still pcm.
                 */
                if (p_sys->sfmt_revert.mFormatID > 0 && p_sys->sfmt_revert.mFormatID != current_streamformat.mFormatID &&
                        p_streams[i] == p_sys->i_stream_id) {
                    msg_Warn(p_aout, STREAM_FORMAT_MSG("Detected current stream format: ", current_streamformat));
                    msg_Warn(p_aout, "... there is another stream format already stored, the current one is ignored");
                } else {
                    p_sys->sfmt_revert = current_streamformat;
                }
                p_sys->b_revert = true;
            }
            p_sys->i_stream_id = p_streams[i];
            p_sys->i_stream_index = i;
            for (int j = 0; j < i_formats; j++) {
                if (p_format_list[j].mFormat.mFormatID == 'IAC3' ||
                   p_format_list[j].mFormat.mFormatID == 'iac3' ||
                   p_format_list[j].mFormat.mFormatID == kAudioFormat60958AC3 ||
                   p_format_list[j].mFormat.mFormatID == kAudioFormatAC3) {
                    if (p_format_list[j].mFormat.mSampleRate == fmt->i_rate) {
                        i_requested_rate_format = j;
                        break;
                    } else if (p_format_list[j].mFormat.mSampleRate == p_sys->sfmt_revert.mSampleRate)
                        i_current_rate_format = j;
                    else {
                        if (i_backup_rate_format < 0 || p_format_list[j].mFormat.mSampleRate > p_format_list[i_backup_rate_format].mFormat.mSampleRate)
                            i_backup_rate_format = j;
                    }
                }
            }
            if (i_requested_rate_format >= 0) /* We prefer to output at the samplerate of the original audio */
                desired_stream_format = p_format_list[i_requested_rate_format].mFormat;
            else if (i_current_rate_format >= 0) /* If not possible, we will try to use the current samplerate of the device */
                desired_stream_format = p_format_list[i_current_rate_format].mFormat;
            else
                desired_stream_format = p_format_list[i_backup_rate_format].mFormat; /* And if we have to, any digital format will be just fine (highest rate possible) */
        }
        free(p_format_list);
    }
    free(p_streams);
    msg_Dbg(p_aout, STREAM_FORMAT_MSG("original stream format: ", p_sys->sfmt_revert));
    if (!AudioStreamChangeFormat(p_aout, p_sys->i_stream_id, desired_stream_format)) {
        msg_Err(p_aout, "failed to change stream format for SPDIF output");
        return false;
    }
    /* Set the format flags */
    if (desired_stream_format.mFormatFlags & kAudioFormatFlagIsBigEndian)
        fmt->i_format = VLC_CODEC_SPDIFB;
    else
        fmt->i_format = VLC_CODEC_SPDIFL;
    fmt->i_bytes_per_frame = AOUT_SPDIF_SIZE;
    fmt->i_frame_length = A52_FRAME_NB;
    fmt->i_rate = (unsigned int)desired_stream_format.mSampleRate;
    p_sys->i_rate = fmt->i_rate;
    aout_FormatPrepare(fmt);
    /* Add IOProc callback */
    err = AudioDeviceCreateIOProcID(p_sys->i_selected_dev,
                                   (AudioDeviceIOProc)RenderCallbackSPDIF,
                                   (void *)p_aout,
                                   &p_sys->i_procID);
    if (err != noErr) {
        msg_Err(p_aout, "Failed to create Process ID [%4.4s]", (char *)&err);
        return false;
    }
    /* Start device */
    err = AudioDeviceStart(p_sys->i_selected_dev, p_sys->i_procID);
    if (err != noErr) {
        msg_Err(p_aout, "Failed to start audio device [%4.4s]", (char *)&err);
        err = AudioDeviceDestroyIOProcID(p_sys->i_selected_dev, p_sys->i_procID);
        if (err != noErr)
            msg_Err(p_aout, "Failed to destroy process ID [%4.4s]", (char *)&err);
        return false;
    }
    /* setup circular buffer */
    TPCircularBufferInit(&p_sys->circular_buffer, 200 * AOUT_SPDIF_SIZE);
    return true;
}
