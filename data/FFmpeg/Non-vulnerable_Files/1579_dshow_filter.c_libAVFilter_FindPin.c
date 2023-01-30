long WINAPI
libAVFilter_FindPin(libAVFilter *this, const wchar_t *id, IPin **pin)
{
    libAVPin *found = NULL;
    dshowdebug("libAVFilter_FindPin(%p)\n", this);
    if (!id || !pin)
        return E_POINTER;
    if (!wcscmp(id, L"In")) {
        found = this->pin;
        libAVPin_AddRef(found);
    }
    *pin = (IPin *) found;
    if (!found)
        return VFW_E_NOT_FOUND;
    return S_OK;
}
