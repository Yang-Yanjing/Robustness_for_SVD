static void DirectXDestroyPool(vout_display_t *);
static int DirectXOpen(vout_display_t *vd, video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    assert(!sys->ddobject);
    assert(!sys->display);
    assert(!sys->clipper);
    /* Initialise DirectDraw */
    if (DirectXOpenDDraw(vd)) {
        msg_Err(vd, "cannot initialize DirectX DirectDraw");
        return VLC_EGENERIC;
    }
    /* Create the directx display */
    if (DirectXOpenDisplay(vd)) {
        msg_Err(vd, "cannot initialize DirectX DirectDraw");
        return VLC_EGENERIC;
    }
    UpdateRects(vd, NULL, NULL, true);
    /* Create the picture pool */
    if (DirectXCreatePool(vd, &sys->use_overlay, fmt)) {
        msg_Err(vd, "cannot create any DirectX surface");
        return VLC_EGENERIC;
    }
    /* */
    if (sys->use_overlay)
        DirectXUpdateOverlay(vd, NULL);
    EventThreadUseOverlay(sys->event, sys->use_overlay);
    /* Change the window title bar text */
    const char *fallback;
    if (sys->use_overlay)
        fallback = VOUT_TITLE " (hardware YUV overlay DirectX output)";
    else if (vlc_fourcc_IsYUV(fmt->i_chroma))
        fallback = VOUT_TITLE " (hardware YUV DirectX output)";
    else
        fallback = VOUT_TITLE " (software RGB DirectX output)";
    EventThreadUpdateTitle(sys->event, fallback);
    return VLC_SUCCESS;
}
