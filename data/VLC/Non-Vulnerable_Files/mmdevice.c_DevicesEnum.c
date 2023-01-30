};
static int DevicesEnum(audio_output_t *aout, IMMDeviceEnumerator *it)
{
    HRESULT hr;
    IMMDeviceCollection *devs;
    hr = IMMDeviceEnumerator_EnumAudioEndpoints(it, eRender,
                                                DEVICE_STATE_ACTIVE, &devs);
    if (FAILED(hr))
    {
        msg_Warn(aout, "cannot enumerate audio endpoints (error 0x%lx)", hr);
        return -1;
    }
    UINT count;
    hr = IMMDeviceCollection_GetCount(devs, &count);
    if (FAILED(hr))
    {
        msg_Warn(aout, "cannot count audio endpoints (error 0x%lx)", hr);
        count = 0;
    }
    unsigned n = 0;
    for (UINT i = 0; i < count; i++)
    {
        IMMDevice *dev;
        char *id, *name;
        hr = IMMDeviceCollection_Item(devs, i, &dev);
        if (FAILED(hr))
            continue;
        /* Unique device ID */
        LPWSTR devid;
        hr = IMMDevice_GetId(dev, &devid);
        if (FAILED(hr))
        {
            IMMDevice_Release(dev);
            continue;
        }
        id = FromWide(devid);
        CoTaskMemFree(devid);
        name = DeviceName(dev);
        IMMDevice_Release(dev);
        aout_HotplugReport(aout, id, (name != NULL) ? name : id);
        free(name);
        free(id);
        n++;
    }
    IMMDeviceCollection_Release(devs);
    return n;
}
