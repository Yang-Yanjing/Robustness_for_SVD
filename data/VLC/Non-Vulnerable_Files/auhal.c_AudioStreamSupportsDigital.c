 */
static int AudioStreamSupportsDigital(audio_output_t *p_aout, AudioStreamID i_stream_id)
{
    OSStatus                    err = noErr;
    UInt32                      i_param_size = 0;
    AudioStreamRangedDescription *p_format_list = NULL;
    int                         i_formats = 0;
    bool                        b_return = false;
    /* Retrieve all the stream formats supported by each output stream */
    AudioObjectPropertyAddress physicalFormatsAddress = { kAudioStreamPropertyAvailablePhysicalFormats, kAudioObjectPropertyScopeGlobal, 0 };
    err = AudioObjectGetPropertyDataSize(i_stream_id, &physicalFormatsAddress, 0, NULL, &i_param_size);
    if (err != noErr) {
        msg_Err(p_aout, "could not get number of streamformats [%4.4s] (%i)", (char *)&err, (int32_t)err);
        return false;
    }
    i_formats = i_param_size / sizeof(AudioStreamRangedDescription);
    msg_Dbg(p_aout, "found %i stream formats for stream id %i", i_formats, i_stream_id);
    p_format_list = (AudioStreamRangedDescription *)malloc(i_param_size);
    if (p_format_list == NULL)
        return false;
    err = AudioObjectGetPropertyData(i_stream_id, &physicalFormatsAddress, 0, NULL, &i_param_size, p_format_list);
    if (err != noErr) {
        msg_Err(p_aout, "could not get the list of streamformats [%4.4s]", (char *)&err);
        free(p_format_list);
        p_format_list = NULL;
        return false;
    }
    for (int i = 0; i < i_formats; i++) {
#ifndef NDEBUG
        msg_Dbg(p_aout, STREAM_FORMAT_MSG("supported format: ", p_format_list[i].mFormat));
#endif
        if (p_format_list[i].mFormat.mFormatID == 'IAC3' ||
            p_format_list[i].mFormat.mFormatID == 'iac3' ||
            p_format_list[i].mFormat.mFormatID == kAudioFormat60958AC3 ||
            p_format_list[i].mFormat.mFormatID == kAudioFormatAC3)
            b_return = true;
    }
    free(p_format_list);
    return b_return;
}
