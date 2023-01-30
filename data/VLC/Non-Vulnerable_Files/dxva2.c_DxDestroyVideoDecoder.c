}
static void DxDestroyVideoDecoder(vlc_va_sys_t *va)
{
    if (va->decoder)
        IDirectXVideoDecoder_Release(va->decoder);
    va->decoder = NULL;
    for (unsigned i = 0; i < va->surface_count; i++)
        IDirect3DSurface9_Release(va->surface[i].d3d);
    va->surface_count = 0;
}
