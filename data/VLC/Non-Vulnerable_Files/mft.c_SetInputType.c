DEFINE_GUID(CODECAPI_AVLowLatencyMode, 0x9c27891a, 0xed7a, 0x40e1, 0x88, 0xe8, 0xb2, 0x27, 0x27, 0xa0, 0x24, 0xee);
static int SetInputType(decoder_t *p_dec, DWORD stream_id, IMFMediaType **result)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    HRESULT hr;
    *result = NULL;
    IMFMediaType *input_media_type = NULL;
    /* Search a suitable input type for the MFT. */
    int input_type_index = 0;
    bool found = false;
    for (int i = 0; !found; ++i)
    {
        hr = IMFTransform_GetInputAvailableType(p_sys->mft, stream_id, i, &input_media_type);
        if (hr == MF_E_NO_MORE_TYPES)
            break;
        else if (hr == MF_E_TRANSFORM_TYPE_NOT_SET)
        {
            /* The output type must be set before setting the input type for this MFT. */
            return VLC_SUCCESS;
        }
        else if (FAILED(hr))
            goto error;
        GUID subtype;
        hr = IMFMediaType_GetGUID(input_media_type, &MF_MT_SUBTYPE, &subtype);
        if (FAILED(hr))
            goto error;
        if (IsEqualGUID(&subtype, p_sys->subtype))
            found = true;
        if (found)
            input_type_index = i;
        IMFMediaType_Release(input_media_type);
        input_media_type = NULL;
    }
    if (!found)
        goto error;
    hr = IMFTransform_GetInputAvailableType(p_sys->mft, stream_id, input_type_index, &input_media_type);
    if (FAILED(hr))
        goto error;
    if (p_dec->fmt_in.i_cat == VIDEO_ES)
    {
        UINT64 width = p_dec->fmt_in.video.i_width;
        UINT64 height = p_dec->fmt_in.video.i_height;
        UINT64 frame_size = (width << 32) | height;
        hr = IMFMediaType_SetUINT64(input_media_type, &MF_MT_FRAME_SIZE, frame_size);
        if (FAILED(hr))
            goto error;
    }
    else
    {
        hr = IMFMediaType_SetUINT32(input_media_type, &MF_MT_ORIGINAL_WAVE_FORMAT_TAG, p_sys->subtype->Data1);
        if (FAILED(hr))
            goto error;
        if (p_dec->fmt_in.audio.i_rate)
        {
            hr = IMFMediaType_SetUINT32(input_media_type, &MF_MT_AUDIO_SAMPLES_PER_SECOND, p_dec->fmt_in.audio.i_rate);
            if (FAILED(hr))
                goto error;
        }
        if (p_dec->fmt_in.audio.i_channels)
        {
            hr = IMFMediaType_SetUINT32(input_media_type, &MF_MT_AUDIO_NUM_CHANNELS, p_dec->fmt_in.audio.i_channels);
            if (FAILED(hr))
                goto error;
        }
        if (p_dec->fmt_in.audio.i_bitspersample)
        {
            hr = IMFMediaType_SetUINT32(input_media_type, &MF_MT_AUDIO_BITS_PER_SAMPLE, p_dec->fmt_in.audio.i_bitspersample);
            if (FAILED(hr))
                goto error;
        }
        if (p_dec->fmt_in.audio.i_blockalign)
        {
            hr = IMFMediaType_SetUINT32(input_media_type, &MF_MT_AUDIO_BLOCK_ALIGNMENT, p_dec->fmt_in.audio.i_blockalign);
            if (FAILED(hr))
                goto error;
        }
        if (p_dec->fmt_in.i_bitrate)
        {
            hr = IMFMediaType_SetUINT32(input_media_type, &MF_MT_AUDIO_AVG_BYTES_PER_SECOND, p_dec->fmt_in.i_bitrate / 8);
            if (FAILED(hr))
                goto error;
        }
    }
    if (p_dec->fmt_in.i_extra > 0)
    {
        UINT32 blob_size = 0;
        hr = IMFMediaType_GetBlobSize(input_media_type, &MF_MT_USER_DATA, &blob_size);
        /*
         * Do not overwrite existing user data in the input type, this
         * can cause the MFT to reject the type.
         */
        if (hr == MF_E_ATTRIBUTENOTFOUND)
        {
            hr = IMFMediaType_SetBlob(input_media_type, &MF_MT_USER_DATA,
                                      (const UINT8*)p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra);
            if (FAILED(hr))
                goto error;
        }
    }
    hr = IMFTransform_SetInputType(p_sys->mft, stream_id, input_media_type, 0);
    if (FAILED(hr))
        goto error;
    *result = input_media_type;
    return VLC_SUCCESS;
error:
    msg_Err(p_dec, "Error in SetInputType()");
    if (input_media_type)
        IMFMediaType_Release(input_media_type);
    return VLC_EGENERIC;
}
