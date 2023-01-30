static STDMETHODIMP
vlc_MMNotificationClient_OnDeviceRemoved(IMMNotificationClient *this,
                                         LPCWSTR wid)
{
    aout_sys_t *sys = vlc_MMNotificationClient_sys(this);
    audio_output_t *aout = sys->aout;
    char *id = FromWide(wid);
    msg_Dbg(aout, "device %ls removed", wid);
    if (unlikely(id == NULL))
        return E_OUTOFMEMORY;
    aout_HotplugReport(aout, id, NULL);
    free(id);
    return S_OK;
}
