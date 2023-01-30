long WINAPI
libAVEnumPins_Clone(libAVEnumPins *this, libAVEnumPins **pins)
{
    libAVEnumPins *new;
    dshowdebug("libAVEnumPins_Clone(%p)\n", this);
    if (!pins)
        return E_POINTER;
    new = libAVEnumPins_Create(this->pin, this->filter);
    if (!new)
        return E_OUTOFMEMORY;
    new->pos = this->pos;
    *pins = new;
    return S_OK;
}
