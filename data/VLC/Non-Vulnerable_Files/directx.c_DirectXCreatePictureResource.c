}
static int DirectXCreatePictureResource(vout_display_t *vd,
                                        bool *use_overlay,
                                        video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    /* */
    picture_sys_t *picsys = calloc(1, sizeof(*picsys));
    if (unlikely(picsys == NULL))
        return VLC_ENOMEM;
    sys->picsys = picsys;
    /* */
    bool allow_hw_yuv  = sys->can_blit_fourcc &&
                         vlc_fourcc_IsYUV(fmt->i_chroma) &&
                         var_InheritBool(vd, "directx-hw-yuv");
    bool allow_overlay = var_InheritBool(vd, "overlay");
    /* Try to use an yuv surface */
    if (allow_hw_yuv) {
        const vlc_fourcc_t *list = vlc_fourcc_GetYUVFallback(fmt->i_chroma);
        /*  Try with overlay first */
        for (unsigned pass = allow_overlay ? 0 : 1; pass < 2; pass++) {
            for (unsigned i = 0; list[i] != 0; i++) {
                const DWORD fourcc = DirectXGetFourcc(list[i]);
                if (!fourcc)
                    continue;
                if (pass == 0) {
                    if (DirectXCreatePictureResourceYuvOverlay(vd, fmt, fourcc))
                        continue;
                } else {
                    if (DirectXCreatePictureResourceYuv(vd, fmt, fourcc))
                        continue;
                }
                /* */
                *use_overlay = pass == 0;
                fmt->i_chroma = list[i];
                return VLC_SUCCESS;
            }
        }
    }
    /* Try plain RGB */
    return DirectXCreatePictureResourceRgb(vd, fmt);
}
