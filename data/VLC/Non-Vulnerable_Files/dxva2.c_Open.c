}
static int Open(vlc_va_t *va, AVCodecContext *ctx, const es_format_t *fmt)
{
    vlc_va_sys_t *sys = calloc(1, sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    va->sys = sys;
    sys->codec_id = ctx->codec_id;
    (void) fmt;
    /* Load dll*/
    sys->hd3d9_dll = LoadLibrary(TEXT("D3D9.DLL"));
    if (!sys->hd3d9_dll) {
        msg_Warn(va, "cannot load d3d9.dll");
        goto error;
    }
    sys->hdxva2_dll = LoadLibrary(TEXT("DXVA2.DLL"));
    if (!sys->hdxva2_dll) {
        msg_Warn(va, "cannot load dxva2.dll");
        goto error;
    }
    msg_Dbg(va, "DLLs loaded");
    /* */
    if (D3dCreateDevice(va)) {
        msg_Err(va, "Failed to create Direct3D device");
        goto error;
    }
    msg_Dbg(va, "D3dCreateDevice succeed");
    if (D3dCreateDeviceManager(va)) {
        msg_Err(va, "D3dCreateDeviceManager failed");
        goto error;
    }
    if (DxCreateVideoService(va)) {
        msg_Err(va, "DxCreateVideoService failed");
        goto error;
    }
    /* */
    if (DxFindVideoServiceConversion(va, &sys->input, &sys->render)) {
        msg_Err(va, "DxFindVideoServiceConversion failed");
        goto error;
    }
    sys->thread_count = ctx->thread_count;
    /* TODO print the hardware name/vendor for debugging purposes */
    va->description = DxDescribe(sys);
    va->pix_fmt = PIX_FMT_DXVA2_VLD;
    va->setup   = Setup;
    va->get     = Get;
    va->release = Release;
    va->extract = Extract;
    return VLC_SUCCESS;
error:
    Close(va);
    return VLC_EGENERIC;
}
