}
static int SetOutputType(decoder_t *p_dec, DWORD stream_id, IMFMediaType **result)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    HRESULT hr;
    *result = NULL;
    IMFMediaType *output_media_type = NULL;
    /*
     * Enumerate available output types. The list is ordered by
     * preference thus we will use the first one unless YV12/I420 is
     * available for video or float32 for audio.
     */
    int output_type_index = 0;
    bool found = false;
    for (int i = 0; !found; ++i)
    {
        hr = IMFTransform_GetOutputAvailableType(p_sys->mft, stream_id, i, &output_media_type);
        if (hr == MF_E_NO_MORE_TYPES)
            break;
        else if (hr == MF_E_TRANSFORM_TYPE_NOT_SET)
        {
            /* The input type must be set before setting the output type for this MFT. */
            return VLC_SUCCESS;
        }
        else if (FAILED(hr))
            goto error;
        GUID subtype;
        hr = IMFMediaType_GetGUID(output_media_type, &MF_MT_SUBTYPE, &subtype);
        if (FAILED(hr))
            goto error;
        if (p_dec->fmt_in.i_cat == VIDEO_ES)
        {
            if (IsEqualGUID(&subtype, &MFVideoFormat_YV12) || IsEqualGUID(&subtype, &MFVideoFormat_I420))
                found = true;
        }
        else
        {
            UINT32 bits_per_sample;
            hr = IMFMediaType_GetUINT32(output_media_type, &MF_MT_AUDIO_BITS_PER_SAMPLE, &bits_per_sample);
            if (FAILED(hr))
                continue;
            if (bits_per_sample == 32 && IsEqualGUID(&subtype, &MFAudioFormat_Float))
                found = true;
        }
        if (found)
            output_type_index = i;
        IMFMediaType_Release(output_media_type);
        output_media_type = NULL;
    }
    /*
     * It's not an error if we don't find the output type we were
     * looking for, in this case we use the first available type which
     * is the "preferred" output type for this MFT.
     */
    hr = IMFTransform_GetOutputAvailableType(p_sys->mft, stream_id, output_type_index, &output_media_type);
    if (FAILED(hr))
        goto error;
    hr = IMFTransform_SetOutputType(p_sys->mft, stream_id, output_media_type, 0);
    if (FAILED(hr))
        goto error;
    GUID subtype;
    hr = IMFMediaType_GetGUID(output_media_type, &MF_MT_SUBTYPE, &subtype);
    if (FAILED(hr))
        goto error;
    if (p_dec->fmt_in.i_cat == VIDEO_ES)
    {
        p_dec->fmt_out.video = p_dec->fmt_in.video;
        p_dec->fmt_out.i_codec = vlc_fourcc_GetCodec(p_dec->fmt_in.i_cat, subtype.Data1);
    }
    else
    {
        p_dec->fmt_out.audio = p_dec->fmt_in.audio;
        UINT32 bitspersample = 0;
        hr = IMFMediaType_GetUINT32(output_media_type, &MF_MT_AUDIO_BITS_PER_SAMPLE, &bitspersample);
        if (SUCCEEDED(hr) && bitspersample)
            p_dec->fmt_out.audio.i_bitspersample = bitspersample;
        UINT32 channels = 0;
        hr = IMFMediaType_GetUINT32(output_media_type, &MF_MT_AUDIO_NUM_CHANNELS, &channels);
        if (SUCCEEDED(hr) && channels)
            p_dec->fmt_out.audio.i_channels = channels;
        UINT32 rate = 0;
        hr = IMFMediaType_GetUINT32(output_media_type, &MF_MT_AUDIO_SAMPLES_PER_SECOND, &rate);
        if (SUCCEEDED(hr) && rate)
            p_dec->fmt_out.audio.i_rate = rate;
        vlc_fourcc_t fourcc;
        wf_tag_to_fourcc(subtype.Data1, &fourcc, NULL);
        p_dec->fmt_out.i_codec = vlc_fourcc_GetCodecAudio(fourcc, p_dec->fmt_out.audio.i_bitspersample);
        p_dec->fmt_out.audio.i_physical_channels = pi_channels_maps[p_dec->fmt_out.audio.i_channels];
        p_dec->fmt_out.audio.i_original_channels = p_dec->fmt_out.audio.i_physical_channels;
    }
    *result = output_media_type;
    return VLC_SUCCESS;
error:
    msg_Err(p_dec, "Error in SetOutputType()");
    if (output_media_type)
        IMFMediaType_Release(output_media_type);
    return VLC_EGENERIC;
}
