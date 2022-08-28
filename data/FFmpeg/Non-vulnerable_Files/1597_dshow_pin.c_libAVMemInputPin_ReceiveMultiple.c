long WINAPI
libAVMemInputPin_ReceiveMultiple(libAVMemInputPin *this,
                                 IMediaSample **samples, long n, long *nproc)
{
    int i;
    dshowdebug("libAVMemInputPin_ReceiveMultiple(%p)\n", this);
    for (i = 0; i < n; i++)
        libAVMemInputPin_Receive(this, samples[i]);
    *nproc = n;
    return S_OK;
}
