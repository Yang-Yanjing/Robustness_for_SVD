 */
static int Direct3DOpen(vout_display_t *vd, video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3D9 d3dobj = sys->d3dobj;
    if (Direct3DFillPresentationParameters(vd))
        return VLC_EGENERIC;
    // Create the D3DDevice
    LPDIRECT3DDEVICE9 d3ddev;
    UINT AdapterToUse = D3DADAPTER_DEFAULT;
    D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
#ifndef NDEBUG
    // Look for 'NVIDIA PerfHUD' adapter
    // If it is present, override default settings
    for (UINT Adapter=0; Adapter< IDirect3D9_GetAdapterCount(d3dobj); ++Adapter) {
        D3DADAPTER_IDENTIFIER9 Identifier;
        HRESULT Res = IDirect3D9_GetAdapterIdentifier(d3dobj,Adapter,0,&Identifier);
        if (SUCCEEDED(Res) && strstr(Identifier.Description,"PerfHUD") != 0) {
            AdapterToUse = Adapter;
            DeviceType = D3DDEVTYPE_REF;
            break;
        }
    }
#endif
    /* */
    D3DADAPTER_IDENTIFIER9 d3dai;
    if (FAILED(IDirect3D9_GetAdapterIdentifier(d3dobj,AdapterToUse,0, &d3dai))) {
        msg_Warn(vd, "IDirect3D9_GetAdapterIdentifier failed");
    } else {
        msg_Dbg(vd, "Direct3d Device: %s %lu %lu %lu", d3dai.Description,
                d3dai.VendorId, d3dai.DeviceId, d3dai.Revision );
    }
    HRESULT hr = IDirect3D9_CreateDevice(d3dobj, AdapterToUse,
                                         DeviceType, sys->hvideownd,
                                         D3DCREATE_SOFTWARE_VERTEXPROCESSING|
                                         D3DCREATE_MULTITHREADED,
                                         &sys->d3dpp, &d3ddev);
    if (FAILED(hr)) {
       msg_Err(vd, "Could not create the D3D device! (hr=0x%lX)", hr);
       return VLC_EGENERIC;
    }
    sys->d3ddev = d3ddev;
    UpdateRects(vd, NULL, NULL, true);
    if (Direct3DCreateResources(vd, fmt)) {
        msg_Err(vd, "Failed to allocate resources");
        return VLC_EGENERIC;
    }
    /* Change the window title bar text */
    EventThreadUpdateTitle(sys->event, VOUT_TITLE " (Direct3D output)");
    msg_Dbg(vd, "Direct3D device adapter successfully initialized");
    return VLC_SUCCESS;
}
