}
static int Open(vlc_va_t *va, AVCodecContext *ctx, const es_format_t *fmt)
{
    VdpStatus err;
    VdpDecoderProfile profile;
    int level = fmt->i_level;
    if (av_vdpau_get_profile(ctx, &profile))
    {
        msg_Err(va, "unsupported codec %d or profile %d", ctx->codec_id,
                fmt->i_profile);
        return VLC_EGENERIC;
    }
    switch (ctx->codec_id)
    {
        case AV_CODEC_ID_MPEG1VIDEO:
            level = VDP_DECODER_LEVEL_MPEG1_NA;
            break;
        case AV_CODEC_ID_MPEG2VIDEO:
            level = VDP_DECODER_LEVEL_MPEG2_HL;
            break;
        case AV_CODEC_ID_H263:
            level = VDP_DECODER_LEVEL_MPEG4_PART2_ASP_L5;
            break;
        case AV_CODEC_ID_H264:
            if ((fmt->i_profile & FF_PROFILE_H264_INTRA)
             && (fmt->i_level == 11))
                level = VDP_DECODER_LEVEL_H264_1b;
        default:
            break;
    }
    if (!vlc_xlib_init(VLC_OBJECT(va)))
    {
        msg_Err(va, "Xlib is required for VDPAU");
        return VLC_EGENERIC;
    }
    vlc_va_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
       return VLC_ENOMEM;
    sys->context = av_vdpau_alloc_context();
    if (unlikely(sys->context == NULL))
    {
        free(sys);
        return VLC_ENOMEM;
    }
    err = vdp_get_x11(NULL, -1, &sys->vdp, &sys->device);
    if (err != VDP_STATUS_OK)
    {
        av_free(sys->context);
        free(sys);
        return VLC_EGENERIC;
    }
    void *func;
    err = vdp_get_proc_address(sys->vdp, sys->device,
                               VDP_FUNC_ID_DECODER_RENDER, &func);
    if (err != VDP_STATUS_OK)
        goto error;
    sys->context->decoder = VDP_INVALID_HANDLE;
    sys->context->render = func;
    sys->profile = profile;
    /* Check capabilities */
    VdpBool support;
    uint32_t l, mb, w, h;
    if (vdp_video_surface_query_capabilities(sys->vdp, sys->device,
              VDP_CHROMA_TYPE_420, &support, &w, &h) != VDP_STATUS_OK)
        support = VDP_FALSE;
    if (!support)
    {
        msg_Err(va, "video surface format not supported: %s", "YUV 4:2:0");
        goto error;
    }
    msg_Dbg(va, "video surface limits: %"PRIu32"x%"PRIu32, w, h);
    if (w < fmt->video.i_width || h < fmt->video.i_height)
    {
        msg_Err(va, "video surface above limits: %ux%u",
                fmt->video.i_width, fmt->video.i_height);
        goto error;
    }
    if (vdp_decoder_query_capabilities(sys->vdp, sys->device, profile,
                                   &support, &l, &mb, &w, &h) != VDP_STATUS_OK)
        support = VDP_FALSE;
    if (!support)
    {
        msg_Err(va, "decoder profile not supported: %u", profile);
        goto error;
    }
    msg_Dbg(va, "decoder profile limits: level %"PRIu32" mb %"PRIu32" "
            "%"PRIu32"x%"PRIu32, l, mb, w, h);
    if ((int)l < level || w < fmt->video.i_width || h < fmt->video.i_height)
    {
        msg_Err(va, "decoder profile above limits: level %d %ux%u",
                level, fmt->video.i_width, fmt->video.i_height);
        goto error;
    }
    const char *infos;
    if (vdp_get_information_string(sys->vdp, &infos) != VDP_STATUS_OK)
        infos = "VDPAU";
    va->sys = sys;
    va->description = infos;
    va->pix_fmt = AV_PIX_FMT_VDPAU;
    va->setup = Setup;
    va->get = Lock;
    va->release = Unlock;
    va->extract = Copy;
    return VLC_SUCCESS;
error:
    vdp_release_x11(sys->vdp);
    av_free(sys->context);
    free(sys);
    return VLC_EGENERIC;
}
