long WINAPI
libAVEnumPins_Skip(libAVEnumPins *this, unsigned long n)
{
    dshowdebug("libAVEnumPins_Skip(%p)\n", this);
    if (n) 
        return S_FALSE;
    return S_OK;
}
