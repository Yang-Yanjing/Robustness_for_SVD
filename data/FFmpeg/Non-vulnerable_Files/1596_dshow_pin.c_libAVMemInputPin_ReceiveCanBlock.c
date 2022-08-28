long WINAPI
libAVMemInputPin_ReceiveCanBlock(libAVMemInputPin *this)
{
    dshowdebug("libAVMemInputPin_ReceiveCanBlock(%p)\n", this);
    
    return S_FALSE;
}
