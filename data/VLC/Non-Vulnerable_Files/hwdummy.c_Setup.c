}
static int Setup(vlc_va_t *va, void **ctxp, vlc_fourcc_t *chromap,
                 int width, int height)
{
    (void) width; (void) height;
    *ctxp = (AVVDPAUContext *)va->sys;
    *chromap = VLC_CODEC_YV12;
    return VLC_SUCCESS;
}
