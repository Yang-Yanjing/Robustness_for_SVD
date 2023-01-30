}
static void SplitterDelWindow(vout_display_t *vd, vout_window_t *window)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vout_DeleteDisplayWindow(osys->vout, vd, window);
}
