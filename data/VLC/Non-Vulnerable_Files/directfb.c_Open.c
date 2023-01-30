/* */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    if (DirectFBInit(NULL,NULL) != DFB_OK) {
        msg_Err(vd, "Cannot init DirectFB");
        free(sys);
        return VLC_EGENERIC;
    }
    DFBSurfaceDescription dsc;
    dsc.flags = DSDESC_CAPS;
    dsc.caps  = DSCAPS_PRIMARY | DSCAPS_TRIPLE;
#if 0
    dsc.flags |= DSDESC_HEIGHT | DSDESC_WIDTH;
    dsc.width = 352;
    dsc.height = 240;
#endif
    IDirectFB *directfb = NULL;
    if (DirectFBCreate(&directfb) != DFB_OK || !directfb)
        goto error;
    sys->directfb = directfb;
    IDirectFBSurface *primary = NULL;
    if (directfb->CreateSurface(directfb, &dsc, &primary) || !primary)
        goto error;
    sys->primary = primary;
    /* */
    int width;
    int height;
    primary->GetSize(primary, &width, &height);
    vout_display_DeleteWindow(vd, NULL);
    /* */
    video_format_t fmt;
    video_format_ApplyRotation(&fmt, &vd->fmt);
    DFBSurfacePixelFormat pixel_format;
    sys->primary->GetPixelFormat(sys->primary, &pixel_format);
    switch (pixel_format) {
    case DSPF_RGB332:
        fmt.i_chroma = VLC_CODEC_RGB8;
        fmt.i_rmask = 0x7 << 5;
        fmt.i_gmask = 0x7 << 2;
        fmt.i_bmask = 0x3 << 0;
        break;
    case DSPF_RGB16: fmt.i_chroma = VLC_CODEC_RGB16; break;
    case DSPF_RGB24: fmt.i_chroma = VLC_CODEC_RGB24; break;
    case DSPF_RGB32: fmt.i_chroma = VLC_CODEC_RGB32; break;
    default:
        msg_Err(vd, "unknown screen depth %i", pixel_format);
        Close(VLC_OBJECT(vd));
        return VLC_EGENERIC;
    }
    video_format_FixRgb(&fmt);
    fmt.i_width  = width;
    fmt.i_height = height;
    fmt.i_visible_width  = width;
    fmt.i_visible_height = height;
    /* */
    vout_display_info_t info = vd->info;
    info.has_hide_mouse = true;
    /* */
    vd->fmt     = fmt;
    vd->info    = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = NULL;
    /* */
    vout_display_SendEventFullscreen(vd, true);
    vout_display_SendEventDisplaySize(vd, fmt.i_width, fmt.i_height, true);
    return VLC_SUCCESS;
error:
    msg_Err(vd, "Cannot create primary surface");
    Close(VLC_OBJECT(vd));
    return VLC_EGENERIC;
}
