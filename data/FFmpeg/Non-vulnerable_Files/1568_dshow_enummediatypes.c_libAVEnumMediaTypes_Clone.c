long WINAPI
libAVEnumMediaTypes_Clone(libAVEnumMediaTypes *this, libAVEnumMediaTypes **enums)
{
    libAVEnumMediaTypes *new;
    dshowdebug("libAVEnumMediaTypes_Clone(%p)\n", this);
    if (!enums)
        return E_POINTER;
    new = libAVEnumMediaTypes_Create(&this->type);
    if (!new)
        return E_OUTOFMEMORY;
    new->pos = this->pos;
    *enums = new;
    return S_OK;
}
