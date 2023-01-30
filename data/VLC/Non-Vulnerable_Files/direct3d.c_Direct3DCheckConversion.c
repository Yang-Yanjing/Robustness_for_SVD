 */
static int Direct3DCheckConversion(vout_display_t *vd,
                                   D3DFORMAT src, D3DFORMAT dst)
{
    vout_display_sys_t *sys = vd->sys;
    LPDIRECT3D9 d3dobj = sys->d3dobj;
    HRESULT hr;
    /* test whether device can create a surface of that format */
    hr = IDirect3D9_CheckDeviceFormat(d3dobj, D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL, dst, 0,
                                      D3DRTYPE_SURFACE, src);
    if (SUCCEEDED(hr)) {
        /* test whether device can perform color-conversion
        ** from that format to target format
        */
        hr = IDirect3D9_CheckDeviceFormatConversion(d3dobj,
                                                    D3DADAPTER_DEFAULT,
                                                    D3DDEVTYPE_HAL,
                                                    src, dst);
    }
    if (!SUCCEEDED(hr)) {
        if (D3DERR_NOTAVAILABLE != hr)
            msg_Err(vd, "Could not query adapter supported formats. (hr=0x%lX)", hr);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
