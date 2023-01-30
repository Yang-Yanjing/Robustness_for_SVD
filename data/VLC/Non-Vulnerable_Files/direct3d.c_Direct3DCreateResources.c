 */
static int Direct3DCreateResources(vout_display_t *vd, video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    if (Direct3DCreatePool(vd, fmt)) {
        msg_Err(vd, "Direct3D picture pool initialization failed");
        return VLC_EGENERIC;
    }
    if (Direct3DCreateScene(vd, fmt)) {
        msg_Err(vd, "Direct3D scene initialization failed !");
        return VLC_EGENERIC;
    }
    if (Direct3DCreateShaders(vd)) {
        /* Failing to initialize shaders is not fatal. */
        msg_Warn(vd, "Direct3D shaders initialization failed !");
    }
    sys->d3dregion_format = D3DFMT_UNKNOWN;
    for (int i = 0; i < 2; i++) {
        D3DFORMAT fmt = i == 0 ? D3DFMT_A8B8G8R8 : D3DFMT_A8R8G8B8;
        if (SUCCEEDED(IDirect3D9_CheckDeviceFormat(sys->d3dobj,
                                                   D3DADAPTER_DEFAULT,
                                                   D3DDEVTYPE_HAL,
                                                   sys->d3dpp.BackBufferFormat,
                                                   D3DUSAGE_DYNAMIC,
                                                   D3DRTYPE_TEXTURE,
                                                   fmt))) {
            sys->d3dregion_format = fmt;
            break;
        }
    }
    return VLC_SUCCESS;
}
