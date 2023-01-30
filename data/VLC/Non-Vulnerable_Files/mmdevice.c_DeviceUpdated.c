}
static HRESULT DeviceUpdated(audio_output_t *aout, LPCWSTR wid)
{
    aout_sys_t *sys = aout->sys;
    HRESULT hr;
    IMMDevice *dev;
    hr = IMMDeviceEnumerator_GetDevice(sys->it, wid, &dev);
    if (FAILED(hr))
        return hr;
    if (!DeviceIsRender(dev))
    {
        IMMDevice_Release(dev);
        return S_OK;
    }
    char *id = FromWide(wid);
    if (unlikely(id == NULL))
    {
        IMMDevice_Release(dev);
        return E_OUTOFMEMORY;
    }
    char *name = DeviceName(dev);
    IMMDevice_Release(dev);
    aout_HotplugReport(aout, id, (name != NULL) ? name : id);
    free(name);
    free(id);
    return S_OK;
}
