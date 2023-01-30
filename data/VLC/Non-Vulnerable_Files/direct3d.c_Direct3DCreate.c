 */
static int Direct3DCreate(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    sys->hd3d9_dll = LoadLibrary(TEXT("D3D9.DLL"));
    if (!sys->hd3d9_dll) {
        msg_Warn(vd, "cannot load d3d9.dll, aborting");
        return VLC_EGENERIC;
    }
    LPDIRECT3D9 (WINAPI *OurDirect3DCreate9)(UINT SDKVersion);
    OurDirect3DCreate9 =
        (void *)GetProcAddress(sys->hd3d9_dll, "Direct3DCreate9");
    if (!OurDirect3DCreate9) {
        msg_Err(vd, "Cannot locate reference to Direct3DCreate9 ABI in DLL");
        return VLC_EGENERIC;
    }
    /* Create the D3D object. */
    LPDIRECT3D9 d3dobj = OurDirect3DCreate9(D3D_SDK_VERSION);
    if (!d3dobj) {
       msg_Err(vd, "Could not create Direct3D9 instance.");
       return VLC_EGENERIC;
    }
    sys->d3dobj = d3dobj;
    sys->hd3d9x_dll = Direct3DLoadShaderLibrary();
    if (!sys->hd3d9x_dll)
        msg_Warn(vd, "cannot load Direct3D Shader Library; HLSL pixel shading will be disabled.");
    /*
    ** Get device capabilities
    */
    ZeroMemory(&sys->d3dcaps, sizeof(sys->d3dcaps));
    HRESULT hr = IDirect3D9_GetDeviceCaps(d3dobj, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &sys->d3dcaps);
    if (FAILED(hr)) {
       msg_Err(vd, "Could not read adapter capabilities. (hr=0x%lX)", hr);
       return VLC_EGENERIC;
    }
    /* TODO: need to test device capabilities and select the right render function */
    if (!(sys->d3dcaps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES) ||
        !(sys->d3dcaps.TextureFilterCaps & (D3DPTFILTERCAPS_MAGFLINEAR)) ||
        !(sys->d3dcaps.TextureFilterCaps & (D3DPTFILTERCAPS_MINFLINEAR))) {
        msg_Err(vd, "Device does not support stretching from textures.");
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
