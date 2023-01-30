}
static int Init(vlc_va_t *va, void **ctxp, vlc_fourcc_t *chromap,
                int width, int height)
{
    vlc_va_sys_t *sys = va->sys;
    VdpStatus err;
    width = (width + 1) & ~1;
    height = (height + 3) & ~3;
    sys->width = width;
    sys->height = height;
    unsigned surfaces = 2;
    switch (sys->profile)
    {
      case VDP_DECODER_PROFILE_H264_BASELINE:
      case VDP_DECODER_PROFILE_H264_MAIN:
      case VDP_DECODER_PROFILE_H264_HIGH:
        surfaces = 16;
        break;
    }
    vlc_vdp_video_field_t **pool = malloc(sizeof (*pool) * (surfaces + 6));
    if (unlikely(pool == NULL))
        return VLC_ENOMEM;
    unsigned i = 0;
    while (i < surfaces + 5)
    {
        pool[i] = CreateSurface(va);
        if (pool[i] == NULL)
            break;
        i++;
    }
    pool[i] = NULL;
    if (i < surfaces + 3)
    {
        msg_Err(va, "not enough video RAM");
        while (i > 0)
            DestroySurface(pool[--i]);
        free(pool);
        return VLC_ENOMEM;
    }
    sys->pool = pool;
    err = vdp_decoder_create(sys->vdp, sys->device, sys->profile, width,
                             height, surfaces, &sys->context->decoder);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(va, "%s creation failure: %s", "decoder",
                vdp_get_error_string(sys->vdp, err));
        while (i > 0)
            DestroySurface(pool[--i]);
        free(pool);
        sys->context->decoder = VDP_INVALID_HANDLE;
        return VLC_EGENERIC;
    }
    *ctxp = sys->context;
    /* TODO: select better chromas when appropriate */
    *chromap = VLC_CODEC_VDPAU_VIDEO_420;
    return VLC_SUCCESS;
}
