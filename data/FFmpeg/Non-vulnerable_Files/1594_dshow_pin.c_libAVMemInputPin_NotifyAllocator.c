long WINAPI
libAVMemInputPin_NotifyAllocator(libAVMemInputPin *this, IMemAllocator *alloc,
                                 BOOL rdwr)
{
    dshowdebug("libAVMemInputPin_NotifyAllocator(%p)\n", this);
    return S_OK;
}
