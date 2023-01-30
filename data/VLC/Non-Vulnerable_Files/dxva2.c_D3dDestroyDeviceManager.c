 */
static void D3dDestroyDeviceManager(vlc_va_sys_t *va)
{
    if (va->devmng)
        IDirect3DDeviceManager9_Release(va->devmng);
}
