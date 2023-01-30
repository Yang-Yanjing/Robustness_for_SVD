 */
static void Direct3DRenderScene(vout_display_t *vd,
                                d3d_region_t *picture,
                                int subpicture_count,
                                d3d_region_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3DDEVICE9 d3ddev = sys->d3ddev;
    HRESULT hr;
    if (sys->clear_scene) {
        /* Clear the backbuffer and the zbuffer */
        hr = IDirect3DDevice9_Clear(d3ddev, 0, NULL, D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
        if (FAILED(hr)) {
            msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
            return;
        }
        sys->clear_scene = false;
    }
    // Begin the scene
    hr = IDirect3DDevice9_BeginScene(d3ddev);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return;
    }
    Direct3DRenderRegion(vd, picture, true);
    if (subpicture_count > 0)
        IDirect3DDevice9_SetRenderState(d3ddev, D3DRS_ALPHABLENDENABLE, TRUE);
    for (int i = 0; i < subpicture_count; i++) {
        d3d_region_t *r = &subpicture[i];
        if (r->texture)
            Direct3DRenderRegion(vd, r, false);
    }
    if (subpicture_count > 0)
        IDirect3DDevice9_SetRenderState(d3ddev, D3DRS_ALPHABLENDENABLE, FALSE);
    // End the scene
    hr = IDirect3DDevice9_EndScene(d3ddev);
    if (FAILED(hr)) {
        msg_Dbg(vd, "%s:%d (hr=0x%0lX)", __FUNCTION__, __LINE__, hr);
        return;
    }
}
