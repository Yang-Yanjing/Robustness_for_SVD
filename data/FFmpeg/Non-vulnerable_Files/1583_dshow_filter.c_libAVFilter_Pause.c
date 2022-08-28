long WINAPI
libAVFilter_Pause(libAVFilter *this)
{
    dshowdebug("libAVFilter_Pause(%p)\n", this);
    this->state = State_Paused;
    return S_OK;
}
