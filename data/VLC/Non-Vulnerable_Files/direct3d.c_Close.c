 */
static void Close(vlc_object_t *object)
{
    vout_display_t * vd = (vout_display_t *)object;
    var_DelCallback(vd, "video-wallpaper", DesktopCallback, NULL);
    vlc_mutex_destroy(&vd->sys->lock);
    Direct3DClose(vd);
    CommonClean(vd);
    Direct3DDestroy(vd);
    free(vd->sys);
}
