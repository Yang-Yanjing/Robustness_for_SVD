static STDMETHODIMP
vlc_AudioEndpointVolumeCallback_OnNotify(IAudioEndpointVolumeCallback *this,
                                  const PAUDIO_VOLUME_NOTIFICATION_DATA notify)
{
    aout_sys_t *sys = vlc_AudioEndpointVolumeCallback_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "endpoint volume changed");
    EnterCriticalSection(&sys->lock);
    WakeConditionVariable(&sys->work); /* implicit state: endpoint volume */
    LeaveCriticalSection(&sys->lock);
    (void) notify;
    return S_OK;
}
