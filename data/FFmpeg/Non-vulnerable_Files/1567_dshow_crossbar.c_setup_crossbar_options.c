static HRESULT
setup_crossbar_options(IAMCrossbar *cross_bar, enum dshowDeviceType devtype, AVFormatContext *avctx)
{
    struct dshow_ctx *ctx = avctx->priv_data;
    long count_output_pins, count_input_pins;
    int i;
    int log_level = ctx->list_options ? AV_LOG_INFO : AV_LOG_DEBUG;
    int video_input_pin = ctx->crossbar_video_input_pin_number;
    int audio_input_pin = ctx->crossbar_audio_input_pin_number;
    const char *device_name = ctx->device_name[devtype];
    HRESULT hr;
    av_log(avctx, log_level, "Crossbar Switching Information for %s:\n", device_name);
    hr = IAMCrossbar_get_PinCounts(cross_bar, &count_output_pins, &count_input_pins);
    if (hr != S_OK) {
        av_log(avctx, AV_LOG_ERROR, "Unable to get crossbar pin counts\n");
        return hr;
    }
    for (i = 0; i < count_output_pins; i++)
    {
        int j;
        long related_pin, pin_type, route_to_pin;
        hr = IAMCrossbar_get_CrossbarPinInfo(cross_bar, FALSE, i, &related_pin, &pin_type);
        if (pin_type == PhysConn_Video_VideoDecoder) {
            
            if (video_input_pin != -1) {
                av_log(avctx, log_level, "Routing video input from pin %d\n", video_input_pin);
                hr = IAMCrossbar_Route(cross_bar, i, video_input_pin);
                if (hr != S_OK) {
                    av_log(avctx, AV_LOG_ERROR, "Unable to route video input from pin %d\n", video_input_pin);
                    return AVERROR(EIO);
                }
            }
        } else if (pin_type == PhysConn_Audio_AudioDecoder) {
            if (audio_input_pin != -1) {
                av_log(avctx, log_level, "Routing audio input from pin %d\n", audio_input_pin);
                hr = IAMCrossbar_Route(cross_bar, i, audio_input_pin);
                if (hr != S_OK) {
                    av_log(avctx, AV_LOG_ERROR, "Unable to route audio input from pin %d\n", audio_input_pin);
                    return hr;
                }
            }
        } else {
            av_log(avctx, AV_LOG_WARNING, "Unexpected output pin type, please report the type if you want to use this (%s)", GetPhysicalPinName(pin_type));
        }
        hr = IAMCrossbar_get_IsRoutedTo(cross_bar, i, &route_to_pin);
        if (hr != S_OK) {
            av_log(avctx, AV_LOG_ERROR, "Unable to get crossbar is routed to from pin %d\n", i);
            return hr;
        }
        av_log(avctx, log_level, "  Crossbar Output pin %d: \"%s\" related output pin: %ld ", i, GetPhysicalPinName(pin_type), related_pin);
        av_log(avctx, log_level, "current input pin: %ld ", route_to_pin);
        av_log(avctx, log_level, "compatible input pins: ");
        for (j = 0; j < count_input_pins; j++)
        {
            hr = IAMCrossbar_CanRoute(cross_bar, i, j);
            if (hr == S_OK)
                av_log(avctx, log_level ,"%d ", j);
        }
        av_log(avctx, log_level, "\n");
    }
    for (i = 0; i < count_input_pins; i++)
    {
        long related_pin, pin_type;
        hr = IAMCrossbar_get_CrossbarPinInfo(cross_bar, TRUE, i, &related_pin, &pin_type);
        if (hr != S_OK) {
            av_log(avctx, AV_LOG_ERROR, "unable to get crossbar info audio input from pin %d\n", i);
            return hr;
        }
        av_log(avctx, log_level, "  Crossbar Input pin %d - \"%s\" ", i, GetPhysicalPinName(pin_type));
        av_log(avctx, log_level, "related input pin: %ld\n", related_pin);
    }
    return S_OK;
}
