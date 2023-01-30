static STDMETHODIMP
vlc_AudioSessionEvents_OnStateChanged(IAudioSessionEvents *this,
                                      AudioSessionState state)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "state changed: %d", state);
    return S_OK;
}
