static STDMETHODIMP
vlc_AudioSessionEvents_QueryInterface(IAudioSessionEvents *this, REFIID riid,
                                      void **ppv)
{
    if (IsEqualIID(riid, &IID_IUnknown)
     || IsEqualIID(riid, &IID_IAudioSessionEvents))
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
