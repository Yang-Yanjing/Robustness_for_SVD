long WINAPI
libAVPin_Disconnect(libAVPin *this)
{
    dshowdebug("libAVPin_Disconnect(%p)\n", this);
    if (this->filter->state != State_Stopped)
        return VFW_E_NOT_STOPPED;
    if (!this->connectedto)
        return S_FALSE;
    IPin_Release(this->connectedto);
    this->connectedto = NULL;
    return S_OK;
}
