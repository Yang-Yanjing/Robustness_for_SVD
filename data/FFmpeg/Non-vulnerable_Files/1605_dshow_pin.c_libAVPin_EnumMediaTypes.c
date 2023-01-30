long WINAPI
libAVPin_EnumMediaTypes(libAVPin *this, IEnumMediaTypes **enumtypes)
{
    const AM_MEDIA_TYPE *type = NULL;
    libAVEnumMediaTypes *new;
    dshowdebug("libAVPin_EnumMediaTypes(%p)\n", this);
    if (!enumtypes)
        return E_POINTER;
    new = libAVEnumMediaTypes_Create(type);
    if (!new)
        return E_OUTOFMEMORY;
    *enumtypes = (IEnumMediaTypes *) new;
    return S_OK;
}
