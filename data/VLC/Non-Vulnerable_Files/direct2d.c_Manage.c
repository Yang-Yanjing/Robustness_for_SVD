 */
static void Manage(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    CommonManage(vd);
    if (sys->changes & DX_POSITION_CHANGE) {
        D2D_ResizeRenderTarget(vd);
        sys->changes &= ~DX_POSITION_CHANGE;
    }
}
