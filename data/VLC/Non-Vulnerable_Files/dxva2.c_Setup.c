/* */
static int Setup(vlc_va_t *va, void **hw, vlc_fourcc_t *chroma,
                 int width, int height)
{
    vlc_va_sys_t *sys = va->sys;
    if (sys->width == width && sys->height == height && sys->decoder)
        goto ok;
    /* */
    DxDestroyVideoConversion(sys);
    DxDestroyVideoDecoder(sys);
    *hw = NULL;
    *chroma = 0;
    if (width <= 0 || height <= 0)
        return VLC_EGENERIC;
    /* FIXME transmit a video_format_t by VaSetup directly */
    video_format_t fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.i_width = width;
    fmt.i_height = height;
    if (DxCreateVideoDecoder(va, sys->codec_id, &fmt))
        return VLC_EGENERIC;
    /* */
    sys->hw.decoder = sys->decoder;
    sys->hw.cfg = &sys->cfg;
    sys->hw.surface_count = sys->surface_count;
    sys->hw.surface = sys->hw_surface;
    /* */
    DxCreateVideoConversion(sys);
    /* */
ok:
    *hw = &sys->hw;
    const d3d_format_t *output = D3dFindFormat(sys->output);
    *chroma = output->codec;
    return VLC_SUCCESS;
}
