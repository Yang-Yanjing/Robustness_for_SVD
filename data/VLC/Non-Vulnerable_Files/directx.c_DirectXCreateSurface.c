 */
static int DirectXCreateSurface(vout_display_t *vd,
                                LPDIRECTDRAWSURFACE2 *surface,
                                const video_format_t *fmt,
                                DWORD fourcc,
                                bool use_overlay,
                                bool use_sysmem,
                                int backbuffer_count)
{
    vout_display_sys_t *sys = vd->sys;
    DDSURFACEDESC ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize   = sizeof(ddsd);
    ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
    ddsd.dwFlags  = DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.dwWidth  = fmt->i_width;
    ddsd.dwHeight = fmt->i_height;
    if (fourcc) {
        ddsd.dwFlags |= DDSD_PIXELFORMAT;
        ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
        ddsd.ddpfPixelFormat.dwFourCC = fourcc;
    }
    if (use_overlay) {
        ddsd.dwFlags |= DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
        if (backbuffer_count > 0)
            ddsd.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_FLIP;
        if (backbuffer_count > 0) {
            ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
            ddsd.dwBackBufferCount = backbuffer_count;
        }
    } else {
        ddsd.dwFlags |= DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        if (use_sysmem)
            ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        else
            ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    }
    /* Create the video surface */
    LPDIRECTDRAWSURFACE surface_v1;
    if (IDirectDraw2_CreateSurface(sys->ddobject, &ddsd, &surface_v1, NULL) != DD_OK)
        return VLC_EGENERIC;
    /* Now that the surface is created, try to get a newer DirectX interface */
    HRESULT hr = IDirectDrawSurface_QueryInterface(surface_v1,
                                                   &IID_IDirectDrawSurface2,
                                                   (LPVOID *)surface);
    IDirectDrawSurface_Release(surface_v1);
    if (hr != DD_OK) {
        msg_Err(vd, "cannot query IDirectDrawSurface2 interface (error %li)", hr);
        return VLC_EGENERIC;
    }
    if (use_overlay) {
        /* Check the overlay is useable as some graphics cards allow creating
         * several overlays but only one can be used at one time. */
        if (DirectXUpdateOverlay(vd, *surface)) {
            IDirectDrawSurface2_Release(*surface);
            msg_Err(vd, "overlay unuseable (might already be in use)");
            return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
