static STDMETHODIMP
vlc_AudioSessionEvents_OnSimpleVolumeChanged(IAudioSessionEvents *this,
                                             float vol, WINBOOL mute,
                                             LPCGUID ctx)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "simple volume changed: %f, muting %sabled", vol,
            mute ? "en" : "dis");
    EnterCriticalSection(&sys->lock);
    WakeConditionVariable(&sys->work); /* implicit state: vol & mute */
    LeaveCriticalSection(&sys->lock);
    (void) ctx;
    return S_OK;
}
