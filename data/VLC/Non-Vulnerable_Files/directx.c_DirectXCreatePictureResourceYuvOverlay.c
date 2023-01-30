}
static int DirectXCreatePictureResourceYuvOverlay(vout_display_t *vd,
                                                  const video_format_t *fmt,
                                                  DWORD fourcc)
{
    vout_display_sys_t *sys = vd->sys;
    bool allow_3buf    = var_InheritBool(vd, "directx-3buffering");
    /* The overlay surface that we create won't be used to decode directly
     * into it because accessing video memory directly is way to slow (remember
     * that pictures are decoded macroblock per macroblock). Instead the video
     * will be decoded in picture buffers in system memory which will then be
     * memcpy() to the overlay surface. */
    LPDIRECTDRAWSURFACE2 front_surface;
    int ret = VLC_EGENERIC;
    if (allow_3buf) {
        /* Triple buffering rocks! it doesn't have any processing overhead
         * (you don't have to wait for the vsync) and provides for a very nice
         * video quality (no tearing). */
        ret = DirectXCreateSurface(vd, &front_surface, fmt, fourcc, true, false, 2);
    }
    if (ret)
        ret = DirectXCreateSurface(vd, &front_surface, fmt, fourcc, true, false, 0);
    if (ret)
        return VLC_EGENERIC;
    msg_Dbg(vd, "YUV overlay surface created successfully");
    /* Get the back buffer */
    LPDIRECTDRAWSURFACE2 surface;
    DDSCAPS dds_caps;
    ZeroMemory(&dds_caps, sizeof(dds_caps));
    dds_caps.dwCaps = DDSCAPS_BACKBUFFER;
    if (IDirectDrawSurface2_GetAttachedSurface(front_surface, &dds_caps, &surface) != DD_OK) {
        msg_Warn(vd, "Failed to get surface back buffer");
        /* front buffer is the same as back buffer */
        surface = front_surface;
    }
    if (DirectXCheckLockingSurface(front_surface, surface)) {
        DirectXDestroySurface(front_surface);
        return VLC_EGENERIC;
    }
    /* */
    picture_sys_t *picsys = sys->picsys;
    picsys->front_surface = front_surface;
    picsys->surface       = surface;
    picsys->fallback      = NULL;
    return VLC_SUCCESS;
}
