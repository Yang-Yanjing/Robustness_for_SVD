long WINAPI
libAVFilter_Stop(libAVFilter *this)
{
    dshowdebug("libAVFilter_Stop(%p)\n", this);
    this->state = State_Stopped;
    return S_OK;
}
