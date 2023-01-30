static STDMETHODIMP
vlc_MMNotificationClient_OnDefaultDeviceChange(IMMNotificationClient *this,
                                               EDataFlow flow, ERole role,
                                               LPCWSTR wid)
{
    aout_sys_t *sys = vlc_MMNotificationClient_sys(this);
    audio_output_t *aout = sys->aout;
    if (flow != eRender)
        return S_OK;
    if (role != eConsole)
        return S_OK;
    msg_Dbg(aout, "default device changed: %ls", wid); /* TODO? migrate */
    return S_OK;
}
