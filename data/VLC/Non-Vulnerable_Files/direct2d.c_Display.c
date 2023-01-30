 */
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    D2D1_RECT_F r_dest = {
        sys->rect_dest.left,
        sys->rect_dest.top,
        sys->rect_dest.right,
        sys->rect_dest.bottom
    };
    if (sys->d2_render_target && sys->d2_bitmap) {
        ID2D1HwndRenderTarget_BeginDraw(sys->d2_render_target);
        ID2D1HwndRenderTarget_DrawBitmap(sys->d2_render_target,
                                         sys->d2_bitmap,
                                         &r_dest,
                                         1.0f,
                                         D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                                         NULL);
        HRESULT hr = ID2D1HwndRenderTarget_EndDraw(sys->d2_render_target,
                                                   NULL,
                                                   NULL);
        if (hr == D2DERR_RECREATE_TARGET) {
            D2D_DestroyRenderTarget(vd);
            D2D_CreateRenderTarget(vd);
        }
    }
    picture_Release(picture);
    VLC_UNUSED(subpicture);
    CommonDisplay(vd);
}
