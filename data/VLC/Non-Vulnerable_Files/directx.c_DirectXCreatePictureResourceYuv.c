}
static int DirectXCreatePictureResourceYuv(vout_display_t *vd,
                                           const video_format_t *fmt,
                                           DWORD fourcc)
{
    vout_display_sys_t *sys = vd->sys;
    bool allow_sysmem  = var_InheritBool(vd, "directx-use-sysmem");
    /* As we can't have an overlay, we'll try to create a plain offscreen
     * surface. This surface will reside in video memory because there's a
     * better chance then that we'll be able to use some kind of hardware
     * acceleration like rescaling, blitting or YUV->RGB conversions.
     * We then only need to blit this surface onto the main display when we
     * want to display it */
    /* Check if the chroma is supported first. This is required
     * because a few buggy drivers don't mind creating the surface
     * even if they don't know about the chroma. */
    DWORD count;
    if (IDirectDraw2_GetFourCCCodes(sys->ddobject, &count, NULL) != DD_OK)
        return VLC_EGENERIC;
    DWORD *list = calloc(count, sizeof(*list));
    if (!list)
        return VLC_ENOMEM;
    if (IDirectDraw2_GetFourCCCodes(sys->ddobject, &count, list) != DD_OK) {
        free(list);
        return VLC_EGENERIC;
    }
    unsigned index;
    for (index = 0; index < count; index++) {
        if (list[index] == fourcc)
            break;
    }
    free(list);
    if (index >= count)
        return VLC_EGENERIC;
    /* */
    LPDIRECTDRAWSURFACE2 surface;
    if (DirectXCreateSurface(vd, &surface, fmt, fourcc, false, allow_sysmem, 0))
        return VLC_EGENERIC;
    msg_Dbg(vd, "YUV plain surface created successfully");
    if (DirectXCheckLockingSurface(surface, surface)) {
        DirectXDestroySurface(surface);
        return VLC_EGENERIC;
    }
    /* */
    picture_sys_t *picsys = sys->picsys;
    picsys->front_surface = surface;
    picsys->surface       = surface;
    picsys->fallback      = NULL;
    return VLC_SUCCESS;
}
