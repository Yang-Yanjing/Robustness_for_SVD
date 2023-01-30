 */
static int Direct3DReset(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3DDEVICE9 d3ddev = sys->d3ddev;
    if (Direct3DFillPresentationParameters(vd))
        return VLC_EGENERIC;
    /* release all D3D objects */
    Direct3DDestroyResources(vd);
    /* */
    HRESULT hr = IDirect3DDevice9_Reset(d3ddev, &sys->d3dpp);
    if (FAILED(hr)) {
        msg_Err(vd, "%s failed ! (hr=%08lX)", __FUNCTION__, hr);
        return VLC_EGENERIC;
    }
    UpdateRects(vd, NULL, NULL, true);
    /* re-create them */
    if (Direct3DCreateResources(vd, &vd->fmt)) {
        msg_Dbg(vd, "%s failed !", __FUNCTION__);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
