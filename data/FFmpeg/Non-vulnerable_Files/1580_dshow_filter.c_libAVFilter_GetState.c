long WINAPI
libAVFilter_GetState(libAVFilter *this, DWORD ms, FILTER_STATE *state)
{
    dshowdebug("libAVFilter_GetState(%p)\n", this);
    if (!state)
        return E_POINTER;
    *state = this->state;
    return S_OK;
}
