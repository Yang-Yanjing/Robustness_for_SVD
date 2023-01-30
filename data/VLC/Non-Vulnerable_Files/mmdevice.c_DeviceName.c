/** Gets the user-readable device name */
static char *DeviceName(IMMDevice *dev)
{
    IPropertyStore *props;
    char *name = NULL;
    PROPVARIANT v;
    HRESULT hr;
    hr = IMMDevice_OpenPropertyStore(dev, STGM_READ, &props);
    if (FAILED(hr))
        return NULL;
    PropVariantInit(&v);
    hr = IPropertyStore_GetValue(props, &PKEY_Device_FriendlyName, &v);
    if (SUCCEEDED(hr))
    {
        name = FromWide(v.pwszVal);
        PropVariantClear(&v);
    }
    IPropertyStore_Release(props);
    return name;
}
