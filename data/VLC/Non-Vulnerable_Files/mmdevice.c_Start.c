}
static int Start(audio_output_t *aout, audio_sample_format_t *restrict fmt)
{
    aout_sys_t *sys = aout->sys;
    if (sys->dev == NULL)
        return -1;
    aout_stream_t *s = vlc_object_create(aout, sizeof (*s));
    if (unlikely(s == NULL))
        return -1;
    s->owner.device = sys->dev;
    s->owner.activate = ActivateDevice;
    EnterMTA();
    for (;;)
    {
        HRESULT hr;
        sys->module = vlc_module_load(s, "aout stream", NULL, false,
                                      aout_stream_Start, s, fmt, &hr);
        if (hr != AUDCLNT_E_DEVICE_INVALIDATED || DeviceSelect(aout, NULL))
            break;
    }
    LeaveMTA();
    if (sys->module == NULL)
    {
        vlc_object_release(s);
        return -1;
    }
    assert (sys->stream == NULL);
    sys->stream = s;
    return 0;
}
