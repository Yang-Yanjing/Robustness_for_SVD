long WINAPI
libAVPin_QueryPinInfo(libAVPin *this, PIN_INFO *info)
{
    dshowdebug("libAVPin_QueryPinInfo(%p)\n", this);
    if (!info)
        return E_POINTER;
    if (this->filter)
        libAVFilter_AddRef(this->filter);
    info->pFilter = (IBaseFilter *) this->filter;
    info->dir     = PINDIR_INPUT;
    wcscpy(info->achName, L"Capture");
    return S_OK;
}
