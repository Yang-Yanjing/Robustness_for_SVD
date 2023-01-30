static STDMETHODIMP
vlc_AudioEndpointVolumeCallback_QueryInterface(IAudioEndpointVolumeCallback *this,
                                               REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, &IID_IUnknown)
     || IsEqualIID(riid, &IID_IAudioEndpointVolumeCallback))
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
