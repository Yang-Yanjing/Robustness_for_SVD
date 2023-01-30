static STDMETHODIMP
vlc_AudioSessionEvents_OnIconPathChanged(IAudioSessionEvents *this,
                                         LPCWSTR wpath, LPCGUID ctx)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "icon path changed: %ls", wpath);
    (void) ctx;
    return S_OK;
}
