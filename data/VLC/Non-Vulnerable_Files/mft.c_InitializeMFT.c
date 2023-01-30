static void DestroyMFT(decoder_t *p_dec);
static int InitializeMFT(decoder_t *p_dec)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    HRESULT hr;
    IMFAttributes *attributes = NULL;
    hr = IMFTransform_GetAttributes(p_sys->mft, &attributes);
    if (hr != E_NOTIMPL && FAILED(hr))
        goto error;
    if (SUCCEEDED(hr))
    {
        UINT32 is_async = false;
        hr = IMFAttributes_GetUINT32(attributes, &MF_TRANSFORM_ASYNC, &is_async);
        if (hr != MF_E_ATTRIBUTENOTFOUND && FAILED(hr))
            goto error;
        p_sys->is_async = is_async;
        if (p_sys->is_async)
        {
            hr = IMFAttributes_SetUINT32(attributes, &MF_TRANSFORM_ASYNC_UNLOCK, true);
            if (FAILED(hr))
                goto error;
            hr = IMFTransform_QueryInterface(p_sys->mft, &IID_IMFMediaEventGenerator, (void**)&p_sys->event_generator);
            if (FAILED(hr))
                goto error;
        }
    }
    DWORD input_streams_count;
    DWORD output_streams_count;
    hr = IMFTransform_GetStreamCount(p_sys->mft, &input_streams_count, &output_streams_count);
    if (FAILED(hr))
        goto error;
    if (input_streams_count != 1 || output_streams_count != 1)
    {
        msg_Err(p_dec, "MFT decoder should have 1 input stream and 1 output stream.");
        goto error;
    }
    hr = IMFTransform_GetStreamIDs(p_sys->mft, 1, &p_sys->input_stream_id, 1, &p_sys->output_stream_id);
    if (hr == E_NOTIMPL)
    {
        /*
         * This is not an error, it happens if:
         * - there is a fixed number of streams.
         * AND
         * - streams are numbered consecutively from 0 to N-1.
         */
        p_sys->input_stream_id = 0;
        p_sys->output_stream_id = 0;
    }
    else if (FAILED(hr))
        goto error;
    if (SetInputType(p_dec, p_sys->input_stream_id, &p_sys->input_type))
        goto error;
    if (SetOutputType(p_dec, p_sys->output_stream_id, &p_sys->output_type))
        goto error;
    /*
     * The input type was not set by the previous call to
     * SetInputType, try again after setting the output type.
     */
    if (!p_sys->input_type)
        if (SetInputType(p_dec, p_sys->input_stream_id, &p_sys->input_type) || !p_sys->input_type)
            goto error;
    /* This call can be a no-op for some MFT decoders, but it can potentially reduce starting time. */
    hr = IMFTransform_ProcessMessage(p_sys->mft, MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, (ULONG_PTR)0);
    if (FAILED(hr))
        goto error;
    /* This event is required for asynchronous MFTs, optional otherwise. */
    hr = IMFTransform_ProcessMessage(p_sys->mft, MFT_MESSAGE_NOTIFY_START_OF_STREAM, (ULONG_PTR)0);
    if (FAILED(hr))
        goto error;
    if (p_dec->fmt_in.i_codec == VLC_CODEC_H264)
    {
        /* It's not an error if the following call fails. */
        IMFAttributes_SetUINT32(attributes, &CODECAPI_AVLowLatencyMode, true);
        if (p_dec->fmt_in.i_extra)
        {
            int buf_size = p_dec->fmt_in.i_extra + 20;
            uint32_t size = p_dec->fmt_in.i_extra;
            uint8_t *buf = malloc(buf_size);
            if (((uint8_t*)p_dec->fmt_in.p_extra)[0] == 1)
            {
                convert_sps_pps(p_dec, p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra,
                                buf, buf_size,
                                &size, &p_sys->nal_size);
            }
            free(buf);
        }
    }
    return VLC_SUCCESS;
error:
    msg_Err(p_dec, "Error in InitializeMFT()");
    DestroyMFT(p_dec);
    return VLC_EGENERIC;
}
