/* FIXME it is nearly common with VAAPI */
static int Get(vlc_va_t *va, void **opaque, uint8_t **data)
{
    vlc_va_sys_t *sys = va->sys;
    /* Check the device */
    HRESULT hr = IDirect3DDeviceManager9_TestDevice(sys->devmng, sys->device);
    if (hr == DXVA2_E_NEW_VIDEO_DEVICE) {
        if (DxResetVideoDecoder(va))
            return VLC_EGENERIC;
    } else if (FAILED(hr)) {
        msg_Err(va, "IDirect3DDeviceManager9_TestDevice %u", (unsigned)hr);
        return VLC_EGENERIC;
    }
    /* Grab an unused surface, in case none are, try the oldest
     * XXX using the oldest is a workaround in case a problem happens with libavcodec */
    unsigned i, old;
    for (i = 0, old = 0; i < sys->surface_count; i++) {
        vlc_va_surface_t *surface = &sys->surface[i];
        if (!surface->refcount)
            break;
        if (surface->order < sys->surface[old].order)
            old = i;
    }
    if (i >= sys->surface_count)
        i = old;
    vlc_va_surface_t *surface = &sys->surface[i];
    surface->refcount = 1;
    surface->order = sys->surface_order++;
    *data = (void *)surface->d3d;
    *opaque = surface;
    return VLC_SUCCESS;
}
