}
static void DestroyMFT(decoder_t *p_dec)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if (p_sys->event_generator)
        IMFMediaEventGenerator_Release(p_sys->event_generator);
    if (p_sys->input_type)
        IMFMediaType_Release(p_sys->input_type);
    if (p_sys->output_sample)
    {
        IMFMediaBuffer *output_media_buffer = NULL;
        HRESULT hr = IMFSample_GetBufferByIndex(p_sys->output_sample, 0, &output_media_buffer);
        if (SUCCEEDED(hr))
            IMFSample_Release(output_media_buffer);
        IMFSample_Release(p_sys->output_sample);
    }
    if (p_sys->output_type)
        IMFMediaType_Release(p_sys->output_type);
    if (p_sys->mft)
        IMFTransform_Release(p_sys->mft);
    p_sys->event_generator = NULL;
    p_sys->input_type = NULL;
    p_sys->output_sample = NULL;
    p_sys->output_type = NULL;
    p_sys->mft = NULL;
}
