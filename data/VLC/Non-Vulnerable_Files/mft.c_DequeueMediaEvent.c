}
static HRESULT DequeueMediaEvent(decoder_t *p_dec)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    HRESULT hr;
    IMFMediaEvent *event = NULL;
    hr = IMFMediaEventGenerator_GetEvent(p_sys->event_generator, MF_EVENT_FLAG_NO_WAIT, &event);
    if (FAILED(hr))
        return hr;
    MediaEventType event_type;
    hr = IMFMediaEvent_GetType(event, &event_type);
    IMFMediaEvent_Release(event);
    if (FAILED(hr))
        return hr;
    if (event_type == VLC_METransformNeedInput)
        p_sys->pending_input_events += 1;
    else if (event_type == VLC_METransformHaveOutput)
        p_sys->pending_output_events += 1;
    else
        msg_Err(p_dec, "Unsupported asynchronous event.");
    return S_OK;
}
