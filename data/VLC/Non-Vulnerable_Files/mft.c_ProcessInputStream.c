}
static int ProcessInputStream(decoder_t *p_dec, DWORD stream_id, block_t *p_block)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    HRESULT hr;
    IMFSample *input_sample = NULL;
    if (AllocateInputSample(p_dec, stream_id, &input_sample, p_block->i_buffer))
        goto error;
    IMFMediaBuffer *input_media_buffer = NULL;
    hr = IMFSample_GetBufferByIndex(input_sample, stream_id, &input_media_buffer);
    if (FAILED(hr))
        goto error;
    BYTE *buffer_start;
    hr = IMFMediaBuffer_Lock(input_media_buffer, &buffer_start, NULL, NULL);
    if (FAILED(hr))
        goto error;
    memcpy(buffer_start, p_block->p_buffer, p_block->i_buffer);
    if (p_dec->fmt_in.i_codec == VLC_CODEC_H264)
    {
        /* in-place NAL to annex B conversion. */
        struct H264ConvertState convert_state = { 0, 0 };
        convert_h264_to_annexb(buffer_start, p_block->i_buffer, p_sys->nal_size, &convert_state);
    }
    hr = IMFMediaBuffer_Unlock(input_media_buffer);
    if (FAILED(hr))
        goto error;
    hr = IMFMediaBuffer_SetCurrentLength(input_media_buffer, p_block->i_buffer);
    if (FAILED(hr))
        goto error;
    LONGLONG ts = p_block->i_pts;
    if (!ts && p_block->i_dts)
        ts = p_block->i_dts;
    /* Convert from microseconds to 100 nanoseconds unit. */
    hr = IMFSample_SetSampleTime(input_sample, ts * 10);
    if (FAILED(hr))
        goto error;
    hr = IMFTransform_ProcessInput(p_sys->mft, stream_id, input_sample, 0);
    if (FAILED(hr))
        goto error;
    IMFMediaBuffer_Release(input_media_buffer);
    IMFSample_Release(input_sample);
    return VLC_SUCCESS;
error:
    msg_Err(p_dec, "Error in ProcessInputStream()");
    if (input_sample)
        IMFSample_Release(input_sample);
    return VLC_EGENERIC;
}
