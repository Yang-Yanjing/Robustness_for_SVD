 */
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    var_DelCallback(vd, "video-wallpaper", WallpaperCallback, NULL);
    vlc_mutex_destroy(&sys->lock);
    /* Make sure the wallpaper is restored */
    WallpaperChange(vd, false);
    DirectXClose(vd);
    CommonClean(vd);
    if (sys->hddraw_dll)
        FreeLibrary(sys->hddraw_dll);
    free(sys);
}
