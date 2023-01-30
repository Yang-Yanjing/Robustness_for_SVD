}
static int DirectXCreatePictureResourceRgb(vout_display_t *vd,
                                           video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    bool allow_sysmem  = var_InheritBool(vd, "directx-use-sysmem");
    /* Our last choice is to use a plain RGB surface */
    DDPIXELFORMAT ddpfPixelFormat;
    ZeroMemory(&ddpfPixelFormat, sizeof(ddpfPixelFormat));
    ddpfPixelFormat.dwSize = sizeof(ddpfPixelFormat);
    IDirectDrawSurface2_GetPixelFormat(sys->display, &ddpfPixelFormat);
    if ((ddpfPixelFormat.dwFlags & DDPF_RGB) == 0)
        return VLC_EGENERIC;
    switch (ddpfPixelFormat.dwRGBBitCount) {
    case 8:
        fmt->i_chroma = VLC_CODEC_RGB8;
        break;
    case 15:
        fmt->i_chroma = VLC_CODEC_RGB15;
        break;
    case 16:
        fmt->i_chroma = VLC_CODEC_RGB16;
        break;
    case 24:
        fmt->i_chroma = VLC_CODEC_RGB24;
        break;
    case 32:
        fmt->i_chroma = VLC_CODEC_RGB32;
        break;
    default:
        msg_Err(vd, "unknown screen depth");
        return VLC_EGENERIC;
    }
    fmt->i_rmask = ddpfPixelFormat.dwRBitMask;
    fmt->i_gmask = ddpfPixelFormat.dwGBitMask;
    fmt->i_bmask = ddpfPixelFormat.dwBBitMask;
    /* */
    LPDIRECTDRAWSURFACE2 surface;
    int ret = DirectXCreateSurface(vd, &surface, fmt, 0, false, allow_sysmem, 0);
    if (ret && !allow_sysmem)
        ret = DirectXCreateSurface(vd, &surface, fmt, 0, false, true, 0);
    if (ret)
        return VLC_EGENERIC;
    msg_Dbg(vd, "RGB plain surface created successfully");
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
