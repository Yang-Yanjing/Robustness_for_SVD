static STDMETHODIMP
vlc_AudioVolumeDuckNotification_OnVolumeDuckNotification(
    IAudioVolumeDuckNotification *this, LPCWSTR sid, UINT32 count)
{
    aout_sys_t *sys = vlc_AudioVolumeDuckNotification_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "volume ducked by %ls of %u sessions", sid, count);
    sys->ducks++;
    aout_PolicyReport(aout, true);
    return S_OK;
}
