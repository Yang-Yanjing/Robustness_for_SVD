static STDMETHODIMP
vlc_MMNotificationClient_OnDeviceAdded(IMMNotificationClient *this,
                                       LPCWSTR wid)
{
    aout_sys_t *sys = vlc_MMNotificationClient_sys(this);
    audio_output_t *aout = sys->aout;
    msg_Dbg(aout, "device %ls added", wid);
    return DeviceUpdated(aout, wid);
}
