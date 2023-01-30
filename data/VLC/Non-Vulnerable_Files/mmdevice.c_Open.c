}
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    aout->sys = sys;
    sys->stream = NULL;
    sys->aout = aout;
    sys->it = NULL;
    sys->dev = NULL;
    sys->device_events.lpVtbl = &vlc_MMNotificationClient;
    sys->endpoint_callback.lpVtbl = &vlc_AudioEndpointVolumeCallback;
    sys->session_events.lpVtbl = &vlc_AudioSessionEvents;
    sys->duck.lpVtbl = &vlc_AudioVolumeDuckNotification;
    sys->refs = 1;
    sys->ducks = 0;
    sys->device = default_device;
    sys->volume = -1.f;
    sys->mute = -1;
    InitializeCriticalSection(&sys->lock);
    InitializeConditionVariable(&sys->work);
    InitializeConditionVariable(&sys->ready);
    /* Initialize MMDevice API */
    if (TryEnterMTA(aout))
        goto error;
    void *pv;
    HRESULT hr = CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL,
                                  &IID_IMMDeviceEnumerator, &pv);
    if (FAILED(hr))
    {
        LeaveMTA();
        msg_Dbg(aout, "cannot create device enumerator (error 0x%lx)", hr);
        goto error;
    }
    sys->it = pv;
    if (vlc_clone(&sys->thread, MMThread, aout, VLC_THREAD_PRIORITY_LOW))
    {
        IMMDeviceEnumerator_Release(sys->it);
        LeaveMTA();
        goto error;
    }
    EnterCriticalSection(&sys->lock);
    while (sys->device != NULL)
        SleepConditionVariableCS(&sys->ready, &sys->lock, INFINITE);
    LeaveCriticalSection(&sys->lock);
    LeaveMTA(); /* Leave MTA after thread has entered MTA */
    aout->start = Start;
    aout->stop = Stop;
    aout->time_get = TimeGet;
    aout->play = Play;
    aout->pause = Pause;
    aout->flush = Flush;
    aout->volume_set = VolumeSet;
    aout->mute_set = MuteSet;
    aout->device_select = DeviceSelect;
    return VLC_SUCCESS;
error:
    DeleteCriticalSection(&sys->lock);
    free(sys);
    return VLC_EGENERIC;
}
