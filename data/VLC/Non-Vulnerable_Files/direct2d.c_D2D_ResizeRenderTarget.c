 */
static void D2D_ResizeRenderTarget(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    D2D1_SIZE_U size = {
        sys->rect_dest.right - sys->rect_dest.left,
        sys->rect_dest.bottom - sys->rect_dest.top
    };
    HRESULT hr  = ID2D1HwndRenderTarget_Resize(sys->d2_render_target, &size);
    if (hr != S_OK)
        msg_Err(vd, "Cannot resize render target (width = %d, height = %d, hr = 0x%x)!",
                size.width, size.height, (unsigned)hr);
}
