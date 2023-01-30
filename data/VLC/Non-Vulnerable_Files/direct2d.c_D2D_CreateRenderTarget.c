 */
static int D2D_CreateRenderTarget(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    sys->d2_render_target = NULL;
    D2D1_PIXEL_FORMAT pf = {
        DXGI_FORMAT_B8G8R8A8_UNORM,
        D2D1_ALPHA_MODE_IGNORE
    };
    D2D1_RENDER_TARGET_PROPERTIES rtp = {
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        pf,
        0,
        0,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT
    };
    D2D1_SIZE_U size = {
        sys->rect_dest.right - sys->rect_dest.left,
        sys->rect_dest.bottom - sys->rect_dest.top
    };
    D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp = {
        sys->hvideownd,
        size,
        D2D1_PRESENT_OPTIONS_IMMEDIATELY /* this might need fiddling */
    };
    HRESULT hr  = ID2D1Factory_CreateHwndRenderTarget(sys->d2_factory,
                                                      &rtp,
                                                      &hrtp,
                                                      &sys->d2_render_target);
    if (hr != S_OK) {
        msg_Err(vd, "Cannot create render target (hvideownd = 0x%x, width = %d, height = %d, pf.format = %d, hr = 0x%x)!",
                (unsigned)hrtp.hwnd, hrtp.pixelSize.width,
                hrtp.pixelSize.height, pf.format, (unsigned)hr);
        sys->d2_render_target = NULL;
        return VLC_EGENERIC;
    }
    FLOAT dpi_x, dpi_y;
    ID2D1Factory_GetDesktopDpi(sys->d2_factory,
                               &dpi_x,
                               &dpi_y);
    D2D1_BITMAP_PROPERTIES bp = {
        pf,
        dpi_x,
        dpi_y
    };
    D2D1_SIZE_U bitmap_size = {
        vd->fmt.i_visible_width,
        vd->fmt.i_visible_height
    };
    hr = ID2D1HwndRenderTarget_CreateBitmap(sys->d2_render_target,
                                            bitmap_size,
                                            NULL,
                                            0,
                                            &bp,
                                            &sys->d2_bitmap);
    if (hr != S_OK) {
        msg_Err(vd, "Failed to create bitmap (hr = 0x%x)!", (unsigned)hr);
        sys->d2_bitmap = NULL;
        D2D_DestroyRenderTarget(vd);
        return VLC_EGENERIC;
    }
#ifndef NDEBUG
    msg_Dbg(vd, "Render trgt dbg: dpi = %f, render_target = %p, bitmap = %p",
            dpi_x, sys->d2_render_target, sys->d2_bitmap);
#endif
    return VLC_SUCCESS;
}
