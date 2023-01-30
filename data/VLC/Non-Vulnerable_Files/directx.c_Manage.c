}
static void Manage(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    CommonManage(vd);
    if (sys->changes & DX_POSITION_CHANGE) {
        /* Update overlay */
        if (sys->use_overlay)
            DirectXUpdateOverlay(vd, NULL);
        /* Check if we are still on the same monitor */
        HMONITOR hmon = MonitorFromWindow(sys->hwnd, MONITOR_DEFAULTTONEAREST);
        if (sys->hmonitor != hmon) {
            vout_display_SendEventPicturesInvalid(vd);
        }
        /* */
        sys->changes &= ~DX_POSITION_CHANGE;
    }
    /* Wallpaper mode change */
    vlc_mutex_lock(&sys->lock);
    const bool ch_wallpaper = sys->ch_wallpaper;
    const bool wallpaper_requested = sys->wallpaper_requested;
    sys->ch_wallpaper = false;
    vlc_mutex_unlock(&sys->lock);
    if (ch_wallpaper)
        WallpaperChange(vd, wallpaper_requested);
    /* */
    if (sys->restore_overlay)
        DirectXUpdateOverlay(vd, NULL);
}
