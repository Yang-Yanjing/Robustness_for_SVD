 */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    OSVERSIONINFO winVer;
    winVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if(GetVersionEx(&winVer) && winVer.dwMajorVersion < 6 && !object->b_force)
        return VLC_EGENERIC;
    /* Allocate structure */
    vd->sys = sys = calloc(1, sizeof(vout_display_sys_t));
    if (!sys)
        return VLC_ENOMEM;
    if (Direct3DCreate(vd)) {
        msg_Err(vd, "Direct3D could not be initialized");
        Direct3DDestroy(vd);
        free(sys);
        return VLC_EGENERIC;
    }
    sys->use_desktop = var_CreateGetBool(vd, "video-wallpaper");
    sys->reset_device = false;
    sys->reopen_device = false;
    sys->lost_not_ready = false;
    sys->allow_hw_yuv = var_CreateGetBool(vd, "directx-hw-yuv");
    sys->desktop_save.is_fullscreen = vd->cfg->is_fullscreen;
    sys->desktop_save.is_on_top     = false;
    sys->desktop_save.win.left      = var_InheritInteger(vd, "video-x");
    sys->desktop_save.win.right     = vd->cfg->display.width;
    sys->desktop_save.win.top       = var_InheritInteger(vd, "video-y");
    sys->desktop_save.win.bottom    = vd->cfg->display.height;
    if (CommonInit(vd))
        goto error;
    /* */
    video_format_t fmt;
    if (Direct3DOpen(vd, &fmt)) {
        msg_Err(vd, "Direct3D could not be opened");
        goto error;
    }
    /* */
    vout_display_info_t info = vd->info;
    info.is_slow = true;
    info.has_double_click = true;
    info.has_hide_mouse = false;
    info.has_pictures_invalid = true;
    info.has_event_thread = true;
    if (var_InheritBool(vd, "direct3d-hw-blending") &&
        sys->d3dregion_format != D3DFMT_UNKNOWN &&
        (sys->d3dcaps.SrcBlendCaps  & D3DPBLENDCAPS_SRCALPHA) &&
        (sys->d3dcaps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA) &&
        (sys->d3dcaps.TextureCaps   & D3DPTEXTURECAPS_ALPHA) &&
        (sys->d3dcaps.TextureOpCaps & D3DTEXOPCAPS_SELECTARG1) &&
        (sys->d3dcaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE))
        info.subpicture_chromas = d3d_subpicture_chromas;
    else
        info.subpicture_chromas = NULL;
    /* Interaction */
    vlc_mutex_init(&sys->lock);
    sys->ch_desktop = false;
    sys->desktop_requested = sys->use_desktop;
    vlc_value_t val;
    val.psz_string = _("Desktop");
    var_Change(vd, "video-wallpaper", VLC_VAR_SETTEXT, &val, NULL);
    var_AddCallback(vd, "video-wallpaper", DesktopCallback, NULL);
    /* Setup vout_display now that everything is fine */
    video_format_Clean(&vd->fmt);
    video_format_Copy(&vd->fmt, &fmt);
    vd->info = info;
    vd->pool    = Pool;
    vd->prepare = Prepare;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = Manage;
    /* Fix state in case of desktop mode */
    if (sys->use_desktop && vd->cfg->is_fullscreen)
        vout_display_SendEventFullscreen(vd, false);
    return VLC_SUCCESS;
error:
    Direct3DClose(vd);
    CommonClean(vd);
    Direct3DDestroy(vd);
    free(vd->sys);
    return VLC_EGENERIC;
}
