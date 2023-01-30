long WINAPI
libAVMemInputPin_GetAllocator(libAVMemInputPin *this, IMemAllocator **alloc)
{
    dshowdebug("libAVMemInputPin_GetAllocator(%p)\n", this);
    return VFW_E_NO_ALLOCATOR;
}
