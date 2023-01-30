long WINAPI
libAVEnumMediaTypes_Skip(libAVEnumMediaTypes *this, unsigned long n)
{
    dshowdebug("libAVEnumMediaTypes_Skip(%p)\n", this);
    if (n) 
        return S_FALSE;
    return S_OK;
}
