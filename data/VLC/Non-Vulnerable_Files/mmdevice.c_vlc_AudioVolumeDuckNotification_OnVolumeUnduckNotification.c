static STDMETHODIMP
vlc_AudioVolumeDuckNotification_OnVolumeUnduckNotification(
    IAudioVolumeDuckNotification *this, LPCWSTR sid)
{
    aout_sys_t *sys = vlc_AudioVolumeDuckNotification_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "volume unducked by %ls", sid);
    sys->ducks--;
    aout_PolicyReport(aout, sys->ducks != 0);
    return S_OK;
}
