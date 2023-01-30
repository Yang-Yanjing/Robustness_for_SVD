 */
static void D2D_DestroyRenderTarget(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->d2_render_target) {
        ID2D1HwndRenderTarget_Release(sys->d2_render_target);
        sys->d2_render_target = NULL;
    }
    if (sys->d2_bitmap) {
        ID2D1Bitmap_Release(sys->d2_bitmap);
        sys->d2_bitmap = NULL;
    }
#ifndef NDEBUG
    msg_Dbg(vd, "Destroyed");
#endif
}
