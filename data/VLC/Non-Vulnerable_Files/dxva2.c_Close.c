}
static void Close(vlc_va_t *va)
{
    vlc_va_sys_t *sys = va->sys;
    DxDestroyVideoConversion(sys);
    DxDestroyVideoDecoder(sys);
    DxDestroyVideoService(sys);
    D3dDestroyDeviceManager(sys);
    D3dDestroyDevice(sys);
    if (sys->hdxva2_dll)
        FreeLibrary(sys->hdxva2_dll);
    if (sys->hd3d9_dll)
        FreeLibrary(sys->hd3d9_dll);
    free((char *)va->description);
    free(sys);
}
