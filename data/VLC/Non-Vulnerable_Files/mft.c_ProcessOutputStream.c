}
static int ProcessOutputStream(decoder_t *p_dec, DWORD stream_id, void **result)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    HRESULT hr;
    picture_t *picture = NULL;
    block_t *aout_buffer = NULL;
    *result = NULL;
    DWORD output_status = 0;
    MFT_OUTPUT_DATA_BUFFER output_buffer = { stream_id, p_sys->output_sample, 0, NULL };
    hr = IMFTransform_ProcessOutput(p_sys->mft, 0, 1, &output_buffer, &output_status);
    if (output_buffer.pEvents)
        IMFCollection_Release(output_buffer.pEvents);
    /* Use the returned sample since it can be provided by the MFT. */
    IMFSample *output_sample = output_buffer.pSample;
    if (hr == S_OK)
    {
        if (!output_sample)
            return VLC_SUCCESS;
        LONGLONG sample_time;
        hr = IMFSample_GetSampleTime(output_sample, &sample_time);
        if (FAILED(hr))
            goto error;
        /* Convert from 100 nanoseconds unit to microseconds. */
        sample_time /= 10;
        DWORD total_length = 0;
        hr = IMFSample_GetTotalLength(output_sample, &total_length);
        if (FAILED(hr))
            goto error;
        if (p_dec->fmt_in.i_cat == VIDEO_ES)
        {
            picture = decoder_NewPicture(p_dec);
            if (!picture)
                return VLC_SUCCESS;
            UINT32 interlaced = false;
            hr = IMFSample_GetUINT32(output_sample, &MFSampleExtension_Interlaced, &interlaced);
            picture->b_progressive = !interlaced;
            picture->date = sample_time;
        }
        else
        {
            if (p_dec->fmt_out.audio.i_bitspersample == 0 || p_dec->fmt_out.audio.i_channels == 0)
                goto error;
            int samples = total_length / (p_dec->fmt_out.audio.i_bitspersample * p_dec->fmt_out.audio.i_channels / 8);
            aout_buffer = decoder_NewAudioBuffer(p_dec, samples);
            if (!aout_buffer)
                return VLC_SUCCESS;
            if (aout_buffer->i_buffer < total_length)
                goto error;
            aout_buffer->i_pts = sample_time;
        }
        IMFMediaBuffer *output_media_buffer = NULL;
        hr = IMFSample_GetBufferByIndex(output_sample, 0, &output_media_buffer);
        BYTE *buffer_start;
        hr = IMFMediaBuffer_Lock(output_media_buffer, &buffer_start, NULL, NULL);
        if (FAILED(hr))
            goto error;
        if (p_dec->fmt_in.i_cat == VIDEO_ES)
            CopyPackedBufferToPicture(picture, buffer_start);
        else
            memcpy(aout_buffer->p_buffer, buffer_start, total_length);
        hr = IMFMediaBuffer_Unlock(output_media_buffer);
        IMFSample_Release(output_media_buffer);
        if (FAILED(hr))
            goto error;
        if (p_sys->output_sample)
        {
            /* Sample is not provided by the MFT: clear its content. */
            hr = IMFMediaBuffer_SetCurrentLength(output_media_buffer, 0);
            if (FAILED(hr))
                goto error;
        }
        else
        {
            /* Sample is provided by the MFT: decrease refcount. */
            IMFSample_Release(output_sample);
        }
    }
    else if (hr == MF_E_TRANSFORM_STREAM_CHANGE || hr == MF_E_TRANSFORM_TYPE_NOT_SET)
    {
        if (p_sys->output_type)
            IMFMediaType_Release(p_sys->output_type);
        if (SetOutputType(p_dec, p_sys->output_stream_id, &p_sys->output_type))
            goto error;
        /* Reallocate output sample. */
        if (p_sys->output_sample)
            IMFSample_Release(p_sys->output_sample);
        p_sys->output_sample = NULL;
        if (AllocateOutputSample(p_dec, 0, &p_sys->output_sample))
            goto error;
        return VLC_SUCCESS;
    }
    else if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT)
    {
        return VLC_SUCCESS;
    }
    else /* An error not listed above occurred */
    {
        msg_Err(p_dec, "Unexpected error in IMFTransform::ProcessOutput: %#lx",
                hr);
        goto error;
    }
    if (p_dec->fmt_in.i_cat == VIDEO_ES)
        *result = picture;
    else
        *result = aout_buffer;
    return VLC_SUCCESS;
error:
    msg_Err(p_dec, "Error in ProcessOutputStream()");
    if (picture)
        picture_Release(picture);
    if (aout_buffer)
        block_Release(aout_buffer);
    return VLC_EGENERIC;
}
