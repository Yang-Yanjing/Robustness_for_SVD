}
static int Setup(vlc_va_t *va, void **ctxp, vlc_fourcc_t *chromap,
                 int width, int height)
{
    vlc_va_sys_t *sys = va->sys;
    if (sys->context->decoder != VDP_INVALID_HANDLE)
    {
        if (sys->width == width && sys->height == height)
            return VLC_SUCCESS;
        Deinit(va);
        sys->context->decoder = VDP_INVALID_HANDLE;
    }
    return Init(va, ctxp, chromap, width, height);
}
