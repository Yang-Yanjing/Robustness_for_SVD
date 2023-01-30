long WINAPI
libAVPin_ConnectedTo(libAVPin *this, IPin **pin)
{
    dshowdebug("libAVPin_ConnectedTo(%p)\n", this);
    if (!pin)
        return E_POINTER;
    if (!this->connectedto)
        return VFW_E_NOT_CONNECTED;
    IPin_AddRef(this->connectedto);
    *pin = this->connectedto;
    return S_OK;
}
