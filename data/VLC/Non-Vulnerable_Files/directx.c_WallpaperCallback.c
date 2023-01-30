/* */
static int WallpaperCallback(vlc_object_t *object, char const *cmd,
                             vlc_value_t oldval, vlc_value_t newval, void *data)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(data);
    vlc_mutex_lock(&sys->lock);
    const bool ch_wallpaper = !sys->wallpaper_requested != !newval.b_bool;
    sys->ch_wallpaper |= ch_wallpaper;
    sys->wallpaper_requested = newval.b_bool;
    vlc_mutex_unlock(&sys->lock);
    return VLC_SUCCESS;
}
