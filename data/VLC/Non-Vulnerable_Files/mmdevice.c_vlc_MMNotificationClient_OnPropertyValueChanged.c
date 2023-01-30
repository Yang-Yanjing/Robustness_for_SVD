static STDMETHODIMP
vlc_MMNotificationClient_OnPropertyValueChanged(IMMNotificationClient *this,
                                                LPCWSTR wid,
                                                const PROPERTYKEY key)
{
    aout_sys_t *sys = vlc_MMNotificationClient_sys(this);
    audio_output_t *aout = sys->aout;
    if (key.pid == PKEY_Device_FriendlyName.pid)
    {
        msg_Dbg(aout, "device %ls name changed", wid);
        return DeviceUpdated(aout, wid);
    }
    return S_OK;
}
