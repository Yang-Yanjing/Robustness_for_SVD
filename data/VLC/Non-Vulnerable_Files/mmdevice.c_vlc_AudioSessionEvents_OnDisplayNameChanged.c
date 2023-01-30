static STDMETHODIMP
vlc_AudioSessionEvents_OnDisplayNameChanged(IAudioSessionEvents *this,
                                            LPCWSTR wname, LPCGUID ctx)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "display name changed: %ls", wname);
    (void) ctx;
    return S_OK;
}
