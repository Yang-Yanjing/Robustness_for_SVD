long WINAPI
libAVFilter_EnumPins(libAVFilter *this, IEnumPins **enumpin)
{
    libAVEnumPins *new;
    dshowdebug("libAVFilter_EnumPins(%p)\n", this);
    if (!enumpin)
        return E_POINTER;
    new = libAVEnumPins_Create(this->pin, this);
    if (!new)
        return E_OUTOFMEMORY;
    *enumpin = (IEnumPins *) new;
    return S_OK;
}
