#endif
void vlc_rand_bytes (void *buf, size_t len)
{
    size_t count = len;
    uint8_t *p_buf = (uint8_t *)buf;
    /* fill buffer with pseudo-random data */
    while (count > 0)
    {
        unsigned int val;
        val = rand();
        if (count < sizeof (val))
        {
            memcpy (p_buf, &val, count);
            break;
        }
        memcpy (p_buf, &val, sizeof (val));
        count -= sizeof (val);
        p_buf += sizeof (val);
    }
#if VLC_WINSTORE_APP
    static const WCHAR *className = L"Windows.Security.Cryptography.CryptographicBuffer";
    const UINT32 clen = wcslen(className);
    HSTRING hClassName = NULL;
    HSTRING_HEADER header;
    HRESULT hr = WindowsCreateStringReference(className, clen, &header, &hClassName);
    if (hr) {
        WindowsDeleteString(hClassName);
        return;
    }
    ICryptographicBufferStatics *cryptoStatics = NULL;
    hr = RoGetActivationFactory(hClassName, &IID_ICryptographicBufferStatics, (void**)&cryptoStatics);
    WindowsDeleteString(hClassName);
    if (hr)
        return;
    IBuffer *buffer = NULL;
    hr = ICryptographicBufferStatics_GenerateRandom(cryptoStatics, len, &buffer);
    if (hr) {
        ICryptographicBufferStatics_Release(cryptoStatics);
        return;
    }
    UINT32 olength;
    unsigned char *rnd = NULL;
    hr = ICryptographicBufferStatics_CopyToByteArray(cryptoStatics, buffer, &olength, (BYTE**)&rnd);
    memcpy(buf, rnd, len);
    IBuffer_Release(buffer);
    ICryptographicBufferStatics_Release(cryptoStatics);
#else
    HCRYPTPROV hProv;
    /* acquire default encryption context */
    if( CryptAcquireContext(
        &hProv,                 // Variable to hold returned handle.
        NULL,                   // Use default key container.
        MS_DEF_PROV,            // Use default CSP.
        PROV_RSA_FULL,          // Type of provider to acquire.
        CRYPT_VERIFYCONTEXT) )  // Flag values
    {
        /* fill buffer with pseudo-random data, intial buffer content
           is used as auxillary random seed */
        CryptGenRandom(hProv, len, buf);
        CryptReleaseContext(hProv, 0);
    }
#endif /* VLC_WINSTORE_APP */
}
