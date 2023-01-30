 */
static void DxDestroyVideoService(vlc_va_sys_t *va)
{
    if (va->device)
        IDirect3DDeviceManager9_CloseDeviceHandle(va->devmng, va->device);
    if (va->vs)
        IDirectXVideoDecoderService_Release(va->vs);
}
