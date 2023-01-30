 */
static int DxCreateVideoDecoder(vlc_va_t *va,
                                int codec_id, const video_format_t *fmt)
{
    vlc_va_sys_t *sys = va->sys;
    /* */
    msg_Dbg(va, "DxCreateVideoDecoder id %d %dx%d",
            codec_id, fmt->i_width, fmt->i_height);
    sys->width  = fmt->i_width;
    sys->height = fmt->i_height;
    /* Allocates all surfaces needed for the decoder */
    sys->surface_width  = (fmt->i_width  + 15) & ~15;
    sys->surface_height = (fmt->i_height + 15) & ~15;
    int surface_count;
    switch (codec_id) {
    case AV_CODEC_ID_H264:
        surface_count = 16 + sys->thread_count + 2;
        break;
    case AV_CODEC_ID_MPEG1VIDEO:
    case AV_CODEC_ID_MPEG2VIDEO:
        surface_count = 2 + 2;
        break;
    default:
        surface_count = 2 + 1;
        break;
    }
    if (surface_count > VA_DXVA2_MAX_SURFACE_COUNT)
        return VLC_EGENERIC;
    sys->surface_count = surface_count;
    if (FAILED(IDirectXVideoDecoderService_CreateSurface(sys->vs,
                                                         sys->surface_width,
                                                         sys->surface_height,
                                                         sys->surface_count - 1,
                                                         sys->render,
                                                         D3DPOOL_DEFAULT,
                                                         0,
                                                         DXVA2_VideoDecoderRenderTarget,
                                                         sys->hw_surface,
                                                         NULL))) {
        msg_Err(va, "IDirectXVideoAccelerationService_CreateSurface failed");
        sys->surface_count = 0;
        return VLC_EGENERIC;
    }
    for (unsigned i = 0; i < sys->surface_count; i++) {
        vlc_va_surface_t *surface = &sys->surface[i];
        surface->d3d = sys->hw_surface[i];
        surface->refcount = 0;
        surface->order = 0;
    }
    msg_Dbg(va, "IDirectXVideoAccelerationService_CreateSurface succeed with %d surfaces (%dx%d)",
            sys->surface_count, fmt->i_width, fmt->i_height);
    /* */
    DXVA2_VideoDesc dsc;
    ZeroMemory(&dsc, sizeof(dsc));
    dsc.SampleWidth     = fmt->i_width;
    dsc.SampleHeight    = fmt->i_height;
    dsc.Format          = sys->render;
    if (fmt->i_frame_rate > 0 && fmt->i_frame_rate_base > 0) {
        dsc.InputSampleFreq.Numerator   = fmt->i_frame_rate;
        dsc.InputSampleFreq.Denominator = fmt->i_frame_rate_base;
    } else {
        dsc.InputSampleFreq.Numerator   = 0;
        dsc.InputSampleFreq.Denominator = 0;
    }
    dsc.OutputFrameFreq = dsc.InputSampleFreq;
    dsc.UABProtectionLevel = FALSE;
    dsc.Reserved = 0;
    /* FIXME I am unsure we can let unknown everywhere */
    DXVA2_ExtendedFormat *ext = &dsc.SampleFormat;
    ext->SampleFormat = 0;//DXVA2_SampleUnknown;
    ext->VideoChromaSubsampling = 0;//DXVA2_VideoChromaSubsampling_Unknown;
    ext->NominalRange = 0;//DXVA2_NominalRange_Unknown;
    ext->VideoTransferMatrix = 0;//DXVA2_VideoTransferMatrix_Unknown;
    ext->VideoLighting = 0;//DXVA2_VideoLighting_Unknown;
    ext->VideoPrimaries = 0;//DXVA2_VideoPrimaries_Unknown;
    ext->VideoTransferFunction = 0;//DXVA2_VideoTransFunc_Unknown;
    /* List all configurations available for the decoder */
    UINT                      cfg_count = 0;
    DXVA2_ConfigPictureDecode *cfg_list = NULL;
    if (FAILED(IDirectXVideoDecoderService_GetDecoderConfigurations(sys->vs,
                                                                    &sys->input,
                                                                    &dsc,
                                                                    NULL,
                                                                    &cfg_count,
                                                                    &cfg_list))) {
        msg_Err(va, "IDirectXVideoDecoderService_GetDecoderConfigurations failed");
        return VLC_EGENERIC;
    }
    msg_Dbg(va, "we got %d decoder configurations", cfg_count);
    /* Select the best decoder configuration */
    int cfg_score = 0;
    for (unsigned i = 0; i < cfg_count; i++) {
        const DXVA2_ConfigPictureDecode *cfg = &cfg_list[i];
        /* */
        msg_Dbg(va, "configuration[%d] ConfigBitstreamRaw %d",
                i, cfg->ConfigBitstreamRaw);
        /* */
        int score;
        if (cfg->ConfigBitstreamRaw == 1)
            score = 1;
        else if (codec_id == AV_CODEC_ID_H264 && cfg->ConfigBitstreamRaw == 2)
            score = 2;
        else
            continue;
        if (IsEqualGUID(&cfg->guidConfigBitstreamEncryption, &DXVA_NoEncrypt))
            score += 16;
        if (cfg_score < score) {
            sys->cfg = *cfg;
            cfg_score = score;
        }
    }
    CoTaskMemFree(cfg_list);
    if (cfg_score <= 0) {
        msg_Err(va, "Failed to find a supported decoder configuration");
        return VLC_EGENERIC;
    }
    /* Create the decoder */
    IDirectXVideoDecoder *decoder;
    if (FAILED(IDirectXVideoDecoderService_CreateVideoDecoder(sys->vs,
                                                              &sys->input,
                                                              &dsc,
                                                              &sys->cfg,
                                                              sys->hw_surface,
                                                              sys->surface_count,
                                                              &decoder))) {
        msg_Err(va, "IDirectXVideoDecoderService_CreateVideoDecoder failed");
        return VLC_EGENERIC;
    }
    sys->decoder = decoder;
    msg_Dbg(va, "IDirectXVideoDecoderService_CreateVideoDecoder succeed");
    return VLC_SUCCESS;
}
