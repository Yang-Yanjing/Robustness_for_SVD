static int
dshow_set_audio_buffer_size(AVFormatContext *avctx, IPin *pin)
{
    struct dshow_ctx *ctx = avctx->priv_data;
    IAMBufferNegotiation *buffer_negotiation = NULL;
    ALLOCATOR_PROPERTIES props = { -1, -1, -1, -1 };
    IAMStreamConfig *config = NULL;
    AM_MEDIA_TYPE *type = NULL;
    int ret = AVERROR(EIO);
    if (IPin_QueryInterface(pin, &IID_IAMStreamConfig, (void **) &config) != S_OK)
        goto end;
    if (IAMStreamConfig_GetFormat(config, &type) != S_OK)
        goto end;
    if (!IsEqualGUID(&type->formattype, &FORMAT_WaveFormatEx))
        goto end;
    props.cbBuffer = (((WAVEFORMATEX *) type->pbFormat)->nAvgBytesPerSec)
                   * ctx->audio_buffer_size / 1000;
    if (IPin_QueryInterface(pin, &IID_IAMBufferNegotiation, (void **) &buffer_negotiation) != S_OK)
        goto end;
    if (IAMBufferNegotiation_SuggestAllocatorProperties(buffer_negotiation, &props) != S_OK)
        goto end;
    ret = 0;
end:
    if (buffer_negotiation)
        IAMBufferNegotiation_Release(buffer_negotiation);
    if (type) {
        if (type->pbFormat)
            CoTaskMemFree(type->pbFormat);
        CoTaskMemFree(type);
    }
    if (config)
        IAMStreamConfig_Release(config);
    return ret;
}
