}
static int AllocateInputSample(decoder_t *p_dec, DWORD stream_id, IMFSample** result, DWORD size)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    MFHandle *mf = &p_sys->mf_handle;
    HRESULT hr;
    *result = NULL;
    IMFSample *input_sample = NULL;
    MFT_INPUT_STREAM_INFO input_info;
    hr = IMFTransform_GetInputStreamInfo(p_sys->mft, stream_id, &input_info);
    if (FAILED(hr))
        goto error;
    hr = mf->MFCreateSample(&input_sample);
    if (FAILED(hr))
        goto error;
    IMFMediaBuffer *input_media_buffer = NULL;
    DWORD allocation_size = __MAX(input_info.cbSize, size);
    hr = mf->MFCreateMemoryBuffer(allocation_size, &input_media_buffer);
    if (FAILED(hr))
        goto error;
    hr = IMFSample_AddBuffer(input_sample, input_media_buffer);
    IMFMediaBuffer_Release(input_media_buffer);
    if (FAILED(hr))
        goto error;
    *result = input_sample;
    return VLC_SUCCESS;
error:
    msg_Err(p_dec, "Error in AllocateInputSample()");
    if (input_sample)
        IMFSample_Release(input_sample);
    if (input_media_buffer)
        IMFMediaBuffer_Release(input_media_buffer);
    return VLC_EGENERIC;
}
