static STDMETHODIMP
vlc_AudioSessionEvents_OnGroupingParamChanged(IAudioSessionEvents *this,
                                              LPCGUID param, LPCGUID ctx)
{
    aout_sys_t *sys = vlc_AudioSessionEvents_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "grouping parameter changed");
    (void) param;
    (void) ctx;
    return S_OK;
}
