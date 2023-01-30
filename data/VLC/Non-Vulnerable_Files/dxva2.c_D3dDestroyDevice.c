 */
static void D3dDestroyDevice(vlc_va_sys_t *va)
{
    if (va->d3ddev)
        IDirect3DDevice9_Release(va->d3ddev);
    if (va->d3dobj)
        IDirect3D9_Release(va->d3dobj);
}
