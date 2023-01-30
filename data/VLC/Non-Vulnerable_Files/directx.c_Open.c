 */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    /* Allocate structure */
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    /* Load direct draw DLL */
    sys->hddraw_dll = LoadLibrary(_T("DDRAW.DLL"));
    if (!sys->hddraw_dll) {
        msg_Warn(vd, "DirectXInitDDraw failed loading ddraw.dll");
        free(sys);
        return VLC_EGENERIC;
    }
    /* */
    sys->use_wallpaper = var_CreateGetBool(vd, "video-wallpaper");
    /* FIXME */
    sys->use_overlay = false;//var_CreateGetBool(vd, "overlay"); /* FIXME */
    sys->restore_overlay = false;
    var_Create(vd, "directx-device", VLC_VAR_STRING | VLC_VAR_DOINHERIT);
    /* Initialisation */
    if (CommonInit(vd))
        goto error;
    /* */
    video_format_t fmt = vd->fmt;
    if (DirectXOpen(vd, &fmt))
        goto error;
    /* */
    vout_display_info_t info = vd->info;
    info.is_slow = true;
    info.has_double_click = true;
    info.has_hide_mouse = false;
    info.has_pictures_invalid = true;
    info.has_event_thread = true;
    /* Interaction TODO support starting with wallpaper mode */
    vlc_mutex_init(&sys->lock);
    sys->ch_wallpaper = sys->use_wallpaper;
    sys->wallpaper_requested = sys->use_wallpaper;
    sys->use_wallpaper = false;
    vlc_value_t val;
    val.psz_string = _("Wallpaper");
    var_Change(vd, "video-wallpaper", VLC_VAR_SETTEXT, &val, NULL);
    var_AddCallback(vd, "video-wallpaper", WallpaperCallback, NULL);
    /* Setup vout_display now that everything is fine */
    video_format_Clean(&vd->fmt);
    video_format_Copy(&vd->fmt, &fmt);
    vd->info    = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = Manage;
    return VLC_SUCCESS;
error:
    DirectXClose(vd);
    CommonClean(vd);
    if (sys->hddraw_dll)
        FreeLibrary(sys->hddraw_dll);
    free(sys);
    return VLC_EGENERIC;
}
