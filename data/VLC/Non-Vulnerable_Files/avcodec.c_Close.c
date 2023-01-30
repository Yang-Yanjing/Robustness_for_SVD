}
static void Close(vlc_va_t *va)
{
    vlc_va_sys_t *sys = va->sys;
    if (sys->context->decoder != VDP_INVALID_HANDLE)
        Deinit(va);
    vdp_release_x11(sys->vdp);
    av_free(sys->context);
    free(sys);
}
