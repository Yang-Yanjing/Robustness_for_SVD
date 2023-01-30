static STDMETHODIMP
vlc_MMNotificationClient_OnDeviceStateChanged(IMMNotificationClient *this,
                                              LPCWSTR wid, DWORD state)
{
    aout_sys_t *sys = vlc_MMNotificationClient_sys(this);
    audio_output_t *aout = sys->aout;
    /* TODO: show device state / ignore missing devices */
    msg_Dbg(aout, "device %ls state changed %08lx", wid, state);
    return S_OK;
}
