 */
static int DirectXOpenDisplay(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    HRESULT hr;
    /* Now get the primary surface. This surface is what you actually see
     * on your screen */
    DDSURFACEDESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    LPDIRECTDRAWSURFACE display;
    hr = IDirectDraw2_CreateSurface(sys->ddobject, &ddsd, &display, NULL);
    if (hr != DD_OK) {
        msg_Err(vd, "cannot get primary surface (error %li)", hr);
        return VLC_EGENERIC;
    }
    void *ptr;
    hr = IDirectDrawSurface_QueryInterface(display, &IID_IDirectDrawSurface2,
                                           &ptr);
    /* Release the old interface */
    IDirectDrawSurface_Release(display);
    if (hr != DD_OK) {
        msg_Err(vd, "cannot query IDirectDrawSurface2 interface (error %li)", hr);
        sys->display = NULL;
        return VLC_EGENERIC;
    }
    sys->display = ptr;
    /* The clipper will be used only in non-overlay mode */
    DirectXCreateClipper(vd);
    /* Make sure the colorkey will be painted */
    sys->i_colorkey = 1;
    sys->i_rgb_colorkey = DirectXFindColorkey(vd, &sys->i_colorkey);
    return VLC_SUCCESS;
}
