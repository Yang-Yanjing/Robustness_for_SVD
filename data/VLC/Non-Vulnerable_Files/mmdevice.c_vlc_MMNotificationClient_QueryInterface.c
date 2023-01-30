static STDMETHODIMP
vlc_MMNotificationClient_QueryInterface(IMMNotificationClient *this,
                                        REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, &IID_IUnknown)
     || IsEqualIID(riid, &IID_IMMNotificationClient))
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
