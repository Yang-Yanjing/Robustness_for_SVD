 */
static HRESULT MMSession(audio_output_t *aout, IMMDeviceEnumerator *it)
{
    aout_sys_t *sys = aout->sys;
    IAudioSessionManager *manager;
    IAudioSessionControl *control;
    ISimpleAudioVolume *volume;
    IAudioEndpointVolume *endpoint;
    void *pv;
    HRESULT hr;
    float base_volume = 1.f;
    assert(sys->device != NULL);
    assert(sys->dev == NULL);
    if (sys->device != default_device) /* Device selected explicitly */
    {
        msg_Dbg(aout, "using selected device %ls", sys->device);
        hr = IMMDeviceEnumerator_GetDevice(it, sys->device, &sys->dev);
        if (FAILED(hr))
            msg_Err(aout, "cannot get selected device %ls (error 0x%lx)",
                    sys->device, hr);
        free(sys->device);
    }
    else
        hr = AUDCLNT_E_DEVICE_INVALIDATED;
    while (hr == AUDCLNT_E_DEVICE_INVALIDATED)
    {   /* Default device selected by policy and with stream routing.
         * "Do not use eMultimedia" says MSDN. */
        msg_Dbg(aout, "using default device");
        hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(it, eRender,
                                                         eConsole, &sys->dev);
        if (FAILED(hr))
            msg_Err(aout, "cannot get default device (error 0x%lx)", hr);
    }
    sys->device = NULL;
    WakeConditionVariable(&sys->ready);
    if (SUCCEEDED(hr))
    {   /* Report actual device */
        LPWSTR wdevid;
        hr = IMMDevice_GetId(sys->dev, &wdevid);
        if (SUCCEEDED(hr))
        {
            char *id = FromWide(wdevid);
            CoTaskMemFree(wdevid);
            if (likely(id != NULL))
            {
                aout_DeviceReport(aout, id);
                free(id);
            }
        }
    }
    else
    {
        msg_Err(aout, "cannot get device identifier (error 0x%lx)", hr);
        return hr;
    }
    /* Create session manager (for controls even w/o active audio client) */
    hr = IMMDevice_Activate(sys->dev, &IID_IAudioSessionManager,
                            CLSCTX_ALL, NULL, &pv);
    manager = pv;
    if (SUCCEEDED(hr))
    {
        LPCGUID guid = &GUID_VLC_AUD_OUT;
        /* Register session control */
        hr = IAudioSessionManager_GetAudioSessionControl(manager, guid, 0,
                                                         &control);
        if (SUCCEEDED(hr))
        {
            wchar_t *ua = var_InheritWide(aout, "user-agent");
            IAudioSessionControl_SetDisplayName(control, ua, NULL);
            free(ua);
            IAudioSessionControl_RegisterAudioSessionNotification(control,
                                                         &sys->session_events);
        }
        else
            msg_Err(aout, "cannot get session control (error 0x%lx)", hr);
        hr = IAudioSessionManager_GetSimpleAudioVolume(manager, guid, FALSE,
                                                       &volume);
        if (FAILED(hr))
            msg_Err(aout, "cannot get simple volume (error 0x%lx)", hr);
        /* Try to get version 2 (Windows 7) of the manager & control */
        wchar_t *siid = NULL;
        hr = IAudioSessionManager_QueryInterface(manager,
                                              &IID_IAudioSessionControl2, &pv);
        if (SUCCEEDED(hr))
        {
            IAudioSessionControl2 *c2 = pv;
            IAudioSessionControl2_SetDuckingPreference(c2, FALSE);
            hr = IAudioSessionControl2_GetSessionInstanceIdentifier(c2, &siid);
            if (FAILED(hr))
                siid = NULL;
            IAudioSessionControl2_Release(c2);
        }
        else
            msg_Dbg(aout, "version 2 session control unavailable");
        hr = IAudioSessionManager_QueryInterface(manager,
                                              &IID_IAudioSessionManager2, &pv);
        if (SUCCEEDED(hr))
        {
            IAudioSessionManager2 *m2 = pv;
            IAudioSessionManager2_RegisterDuckNotification(m2, siid,
                                                           &sys->duck);
            IAudioSessionManager2_Release(m2);
        }
        else
            msg_Dbg(aout, "version 2 session management unavailable");
        CoTaskMemFree(siid);
    }
    else
    {
        msg_Err(aout, "cannot activate session manager (error 0x%lx)", hr);
        control = NULL;
        volume = NULL;
    }
    hr = IMMDevice_Activate(sys->dev, &IID_IAudioEndpointVolume,
                            CLSCTX_ALL, NULL, &pv);
    endpoint = pv;
    if (SUCCEEDED(hr))
    {
        float min, max, inc;
        hr = IAudioEndpointVolume_GetVolumeRange(endpoint, &min, &max, &inc);
        if (SUCCEEDED(hr))
        {
            msg_Dbg(aout, "volume from %+f dB to %+f dB with %f dB increments",
                    min, max, inc);
            base_volume = powf(10.f, max / 20.f);
        }
        else
            msg_Err(aout, "cannot get volume range (error 0x%lx)", hr);
        IAudioEndpointVolume_RegisterControlChangeNotify(endpoint,
                                                      &sys->endpoint_callback);
    }
    else
        msg_Err(aout, "cannot activate endpoint volume (error %lx)", hr);
    /* Main loop (adjust volume as long as device is unchanged) */
    while (sys->device == NULL)
    {
        float level = 1.f, master = 1.f;
        if (volume != NULL)
        {
            hr = ISimpleAudioVolume_GetMasterVolume(volume, &level);
            if (FAILED(hr))
                msg_Err(aout, "cannot get master volume (error 0x%lx)", hr);
        }
        if (endpoint != NULL)
        {
            float db;
            hr = IAudioEndpointVolume_GetMasterVolumeLevel(endpoint, &db);
            if (SUCCEEDED(hr))
                master = powf(10.f, db / 20.f);
            else
                msg_Err(aout, "cannot get endpoint volume (error 0x%lx)", hr);
        }
        aout_VolumeReport(aout, cbrtf(level * master * base_volume));
    /* The WASAPI simple volume is relative to the endpoint volume, and it
     * cannot exceed 100%. Therefore the endpoint master volume must be
     * increased to reach an overall volume above the current endpoint master
     * volume. Unfortunately, that means the volume of other applications will
     * also be changed (which may or may not be what the user wants) and
     * introduces race conditions between updates. */
        level = sys->volume / base_volume;
        sys->volume = -1.f;
        if (level > master)
        {
            master = level;
            level = 1.f;
        }
        else
        {
            if (master > 0.f)
                level /= master;
            master = -1.f;
        }
        if (volume != NULL)
        {
            if (level >= 0.f)
            {
                assert(level <= 1.f);
                hr = ISimpleAudioVolume_SetMasterVolume(volume, level, NULL);
                if (FAILED(hr))
                    msg_Err(aout, "cannot set master volume (error 0x%lx)",
                            hr);
            }
            BOOL mute;
            hr = ISimpleAudioVolume_GetMute(volume, &mute);
            if (SUCCEEDED(hr))
                aout_MuteReport(aout, mute != FALSE);
            else
                msg_Err(aout, "cannot get mute (error 0x%lx)", hr);
            if (sys->mute >= 0)
            {
                mute = sys->mute ? TRUE : FALSE;
                hr = ISimpleAudioVolume_SetMute(volume, mute, NULL);
                if (FAILED(hr))
                    msg_Err(aout, "cannot set mute (error 0x%lx)", hr);
            }
            sys->mute = -1;
        }
        if (endpoint != NULL && master >= 0.f)
        {
            float v = 20.f * log10f(master);
            msg_Warn(aout, "overriding endpoint volume: %+f dB", v);
            hr = IAudioEndpointVolume_SetMasterVolumeLevel(endpoint, v, NULL);
            if (FAILED(hr))
                msg_Err(aout, "cannot set endpoint volume (error 0x%lx)", hr);
        }
        sys->volume = -1.f;
        SleepConditionVariableCS(&sys->work, &sys->lock, INFINITE);
    }
    LeaveCriticalSection(&sys->lock);
    if (endpoint != NULL)
    {
        IAudioEndpointVolume_UnregisterControlChangeNotify(endpoint,
                                                      &sys->endpoint_callback);
        IAudioEndpointVolume_Release(endpoint);
    }
    if (manager != NULL)
    {   /* Deregister callbacks *without* the lock */
        hr = IAudioSessionManager_QueryInterface(manager,
                                              &IID_IAudioSessionManager2, &pv);
        if (SUCCEEDED(hr))
        {
            IAudioSessionManager2 *m2 = pv;
            IAudioSessionManager2_UnregisterDuckNotification(m2, &sys->duck);
            IAudioSessionManager2_Release(m2);
        }
        if (volume != NULL)
            ISimpleAudioVolume_Release(volume);
        if (control != NULL)
        {
            IAudioSessionControl_UnregisterAudioSessionNotification(control,
                                                         &sys->session_events);
            IAudioSessionControl_Release(control);
        }
        IAudioSessionManager_Release(manager);
    }
    EnterCriticalSection(&sys->lock);
    IMMDevice_Release(sys->dev);
    sys->dev = NULL;
    return S_OK;
}
