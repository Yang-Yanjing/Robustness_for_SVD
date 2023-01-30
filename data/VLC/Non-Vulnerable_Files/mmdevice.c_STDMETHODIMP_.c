}
static STDMETHODIMP_(ULONG)
vlc_AudioSessionEvents_AddRef(IAudioSessionEvents *this)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    return InterlockedIncrement(&sys->refs);
}
}
static STDMETHODIMP_(ULONG)
vlc_AudioSessionEvents_Release(IAudioSessionEvents *this)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    return InterlockedDecrement(&sys->refs);
}
}
static STDMETHODIMP_(ULONG)
vlc_AudioVolumeDuckNotification_AddRef(IAudioVolumeDuckNotification *this)
{
    aout_sys_t *sys = vlc_AudioVolumeDuckNotification_sys(this);
    return InterlockedIncrement(&sys->refs);
}
}
static STDMETHODIMP_(ULONG)
vlc_AudioVolumeDuckNotification_Release(IAudioVolumeDuckNotification *this)
{
    aout_sys_t *sys = vlc_AudioVolumeDuckNotification_sys(this);
    return InterlockedDecrement(&sys->refs);
}
}
static STDMETHODIMP_(ULONG)
vlc_AudioEndpointVolumeCallback_AddRef(IAudioEndpointVolumeCallback *this)
{
    aout_sys_t *sys = vlc_AudioEndpointVolumeCallback_sys(this);
    return InterlockedIncrement(&sys->refs);
}
}
static STDMETHODIMP_(ULONG)
vlc_AudioEndpointVolumeCallback_Release(IAudioEndpointVolumeCallback *this)
{
    aout_sys_t *sys = vlc_AudioEndpointVolumeCallback_sys(this);
    return InterlockedDecrement(&sys->refs);
}
}
static STDMETHODIMP_(ULONG)
vlc_MMNotificationClient_AddRef(IMMNotificationClient *this)
{
    aout_sys_t *sys = vlc_MMNotificationClient_sys(this);
    return InterlockedIncrement(&sys->refs);
}
}
static STDMETHODIMP_(ULONG)
vlc_MMNotificationClient_Release(IMMNotificationClient *this)
{
    aout_sys_t *sys = vlc_MMNotificationClient_sys(this);
    return InterlockedDecrement(&sys->refs);
}
