long WINAPI
libAVFilter_GetSyncSource(libAVFilter *this, IReferenceClock **clock)
{
    dshowdebug("libAVFilter_GetSyncSource(%p)\n", this);
    if (!clock)
        return E_POINTER;
    if (this->clock)
        IReferenceClock_AddRef(this->clock);
    *clock = this->clock;
    return S_OK;
}
