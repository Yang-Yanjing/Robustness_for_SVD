 */
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool)
        picture_pool_Delete(sys->pool);
    if (!sys->is_hw_accel && sys->picture)
        picture_Release(sys->picture);
    CloseDisplay(vd);
    if (sys->is_tty)
        TtyExit(vd);
    free(sys);
}
