HRESULT
dshow_try_setup_crossbar_options(ICaptureGraphBuilder2 *graph_builder2,
    IBaseFilter *device_filter, enum dshowDeviceType devtype, AVFormatContext *avctx)
{
    struct dshow_ctx *ctx = avctx->priv_data;
    IAMCrossbar *cross_bar = NULL;
    IBaseFilter *cross_bar_base_filter = NULL;
    IAMTVTuner *tv_tuner_filter = NULL;
    IBaseFilter *tv_tuner_base_filter = NULL;
    IAMAudioInputMixer *tv_audio_filter = NULL;
    IBaseFilter *tv_audio_base_filter = NULL;
    HRESULT hr;
    hr = ICaptureGraphBuilder2_FindInterface(graph_builder2, &LOOK_UPSTREAM_ONLY, (const GUID *) NULL,
            device_filter, &IID_IAMCrossbar, (void**) &cross_bar);
    if (hr != S_OK) {
        
        hr = S_OK;
        goto end;
    }
    
    if (devtype == VideoDevice && ctx->show_video_crossbar_connection_dialog ||
        devtype == AudioDevice && ctx->show_audio_crossbar_connection_dialog) {
        hr = IAMCrossbar_QueryInterface(cross_bar, &IID_IBaseFilter, (void **) &cross_bar_base_filter);
        if (hr != S_OK)
            goto end;
        dshow_show_filter_properties(cross_bar_base_filter, avctx);
    }
    if (devtype == VideoDevice && ctx->show_analog_tv_tuner_dialog) {
        hr = ICaptureGraphBuilder2_FindInterface(graph_builder2, &LOOK_UPSTREAM_ONLY, NULL, 
             device_filter, &IID_IAMTVTuner, (void**) &tv_tuner_filter);
        if (hr == S_OK) {
            hr = IAMCrossbar_QueryInterface(tv_tuner_filter, &IID_IBaseFilter, (void **) &tv_tuner_base_filter);
            if (hr != S_OK)
                goto end;
            dshow_show_filter_properties(tv_tuner_base_filter, avctx);
        } else {
            av_log(avctx, AV_LOG_WARNING, "unable to find a tv tuner to display dialog for!");
        }
    }
    if (devtype == AudioDevice && ctx->show_analog_tv_tuner_audio_dialog) {
        hr = ICaptureGraphBuilder2_FindInterface(graph_builder2, &LOOK_UPSTREAM_ONLY, NULL, 
             device_filter, &IID_IAMTVAudio, (void**) &tv_audio_filter);
        if (hr == S_OK) {
            hr = IAMCrossbar_QueryInterface(tv_audio_filter, &IID_IBaseFilter, (void **) &tv_audio_base_filter);
            if (hr != S_OK)
                goto end;
            dshow_show_filter_properties(tv_audio_base_filter, avctx);
        } else {
            av_log(avctx, AV_LOG_WARNING, "unable to find a tv audio tuner to display dialog for!");
        }
    }
    hr = setup_crossbar_options(cross_bar, devtype, avctx);
    if (hr != S_OK)
        goto end;
end:
    if (cross_bar)
        IAMCrossbar_Release(cross_bar);
    if (cross_bar_base_filter)
        IBaseFilter_Release(cross_bar_base_filter);
    if (tv_tuner_filter)
        IAMTVTuner_Release(tv_tuner_filter);
    if (tv_tuner_base_filter)
        IBaseFilter_Release(tv_tuner_base_filter);
    return hr;
}
