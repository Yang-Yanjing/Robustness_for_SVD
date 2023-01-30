}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3DDEVICE9 d3ddev = sys->d3ddev;
    if (sys->lost_not_ready) {
        picture_Release(picture);
        if (subpicture)
            subpicture_Delete(subpicture);
        return;
    }
    // Present the back buffer contents to the display
    // No stretching should happen here !
    const RECT src = sys->rect_dest_clipped;
    const RECT dst = sys->rect_dest_clipped;
    HRESULT hr = IDirect3DDevice9_Present(d3ddev, &src, &dst, NULL, NULL);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
    }
#if 0
    VLC_UNUSED(picture);
    VLC_UNUSED(subpicture);
#else
    /* XXX See Prepare() */
    Direct3DLockSurface(picture);
    picture_Release(picture);
#endif
    if (subpicture)
        subpicture_Delete(subpicture);
    CommonDisplay(vd);
}
