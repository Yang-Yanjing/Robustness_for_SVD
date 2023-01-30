static STDMETHODIMP
vlc_AudioVolumeDuckNotification_QueryInterface(
    IAudioVolumeDuckNotification *this, REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, &IID_IUnknown)
     || IsEqualIID(riid, &IID_IAudioVolumeDuckNotification))
    {
        *ppv = this;
        IUnknown_AddRef(this);
        return S_OK;
    }
    else
    {
       *ppv = NULL;
        return E_NOINTERFACE;
    }
}
