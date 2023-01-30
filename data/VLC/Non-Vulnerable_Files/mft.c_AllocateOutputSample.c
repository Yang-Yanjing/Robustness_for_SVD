}
static int AllocateOutputSample(decoder_t *p_dec, DWORD stream_id, IMFSample **result)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    MFHandle *mf = &p_sys->mf_handle;
    HRESULT hr;
    *result = NULL;
    IMFSample *output_sample = NULL;
    MFT_OUTPUT_STREAM_INFO output_info;
    hr = IMFTransform_GetOutputStreamInfo(p_sys->mft, stream_id, &output_info);
    if (FAILED(hr))
        goto error;
    if (output_info.dwFlags & (MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES))
    {
        /* The MFT will provide an allocated sample. */
        return VLC_SUCCESS;
    }
    DWORD expected_flags = 0;
    if (p_dec->fmt_in.i_cat == VIDEO_ES)
        expected_flags |= MFT_OUTPUT_STREAM_WHOLE_SAMPLES
                        | MFT_OUTPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER
                        | MFT_OUTPUT_STREAM_FIXED_SAMPLE_SIZE;
    if ((output_info.dwFlags & expected_flags) != expected_flags)
        goto error;
    hr = mf->MFCreateSample(&output_sample);
    if (FAILED(hr))
        goto error;
    IMFMediaBuffer *output_media_buffer = NULL;
    DWORD allocation_size = output_info.cbSize;
    DWORD alignment = output_info.cbAlignment;
    if (alignment > 0)
        hr = mf->MFCreateAlignedMemoryBuffer(allocation_size, alignment - 1, &output_media_buffer);
    else
        hr = mf->MFCreateMemoryBuffer(allocation_size, &output_media_buffer);
    if (FAILED(hr))
        goto error;
    hr = IMFSample_AddBuffer(output_sample, output_media_buffer);
    if (FAILED(hr))
        goto error;
    *result = output_sample;
    return VLC_SUCCESS;
error:
    msg_Err(p_dec, "Error in AllocateOutputSample()");
    if (output_sample)
        IMFSample_Release(output_sample);
    return VLC_EGENERIC;
}
