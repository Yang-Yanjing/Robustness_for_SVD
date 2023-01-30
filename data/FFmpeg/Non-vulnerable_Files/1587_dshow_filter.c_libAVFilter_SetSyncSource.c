long WINAPI
libAVFilter_SetSyncSource(libAVFilter *this, IReferenceClock *clock)
{
    dshowdebug("libAVFilter_SetSyncSource(%p)\n", this);
    if (this->clock != clock) {
        if (this->clock)
            IReferenceClock_Release(this->clock);
        this->clock = clock;
        if (clock)
            IReferenceClock_AddRef(clock);
    }
    return S_OK;
}
