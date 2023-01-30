}
static int Control(vout_display_t *vd, int query, va_list args)
{
    vout_display_sys_t *sys = vd->sys;
    switch (query) {
    case VOUT_DISPLAY_RESET_PICTURES:
        DirectXClose(vd);
        /* Make sure the wallpaper is restored */
        if (sys->use_wallpaper) {
            vlc_mutex_lock(&sys->lock);
            if (!sys->ch_wallpaper) {
                sys->ch_wallpaper = true;
                sys->wallpaper_requested = true;
            }
            vlc_mutex_unlock(&sys->lock);
            WallpaperChange(vd, false);
        }
        return DirectXOpen(vd, &vd->fmt);
    default:
        return CommonControl(vd, query, args);
    }
}
