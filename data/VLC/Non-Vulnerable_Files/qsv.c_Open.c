}
static int Open(vlc_object_t *this)
{
    encoder_t *enc = (encoder_t *)this;
    encoder_sys_t *sys = NULL;
    mfxStatus sts = MFX_ERR_NONE;
    mfxFrameAllocRequest alloc_request;
    mfxExtCodingOptionSPSPPS headers;
    mfxExtBuffer *extended_params[1] = {(mfxExtBuffer *)&headers};
    uint8_t *p_extra;
    size_t i_extra;
    uint8_t nals[128];
    if (enc->fmt_out.i_codec != VLC_CODEC_H264 &&
        enc->fmt_out.i_codec != VLC_CODEC_MPGV && !enc->b_force)
        return VLC_EGENERIC;
    if (!enc->fmt_in.video.i_visible_height || !enc->fmt_in.video.i_visible_width ||
        !enc->fmt_in.video.i_frame_rate || !enc->fmt_in.video.i_frame_rate_base) {
        msg_Err(enc, "Framerate and picture dimensions must be non-zero");
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    sys = calloc(1, sizeof(encoder_sys_t));
    if (unlikely(!sys))
        return VLC_ENOMEM;
    /* Initialize dispatcher, it will loads the actual SW/HW Implementation */
    sts = MFXInit(MFX_IMPL_AUTO, 0, &sys->session);
    if (sts != MFX_ERR_NONE) {
        msg_Err(enc, "Unable to find an Intel Media SDK implementation.");
        free(sys);
        return VLC_EGENERIC;
    }
    config_ChainParse(enc, SOUT_CFG_PREFIX, sout_options, enc->p_cfg);
    /* Checking if we are on software and are allowing it */
    MFXQueryIMPL(sys->session, &sys->impl);
    if (!var_InheritBool(enc, SOUT_CFG_PREFIX "software") && (sys->impl & MFX_IMPL_SOFTWARE)) {
        msg_Err(enc, "No hardware implementation found and software mode disabled");
        free(sys);
        return VLC_EGENERIC;
    }
    msg_Dbg(enc, "Using Intel QuickSync Video %s implementation",
        sys->impl & MFX_IMPL_HARDWARE ? "hardware" : "software");
    /* Vlc module configuration */
    enc->p_sys                         = sys;
    enc->fmt_in.i_codec                = VLC_CODEC_NV12; // Intel Media SDK requirement
    enc->fmt_in.video.i_bits_per_pixel = 12;
    /* Input picture format description */
    sys->params.mfx.FrameInfo.FrameRateExtN = enc->fmt_in.video.i_frame_rate;
    sys->params.mfx.FrameInfo.FrameRateExtD = enc->fmt_in.video.i_frame_rate_base;
    sys->params.mfx.FrameInfo.FourCC        = MFX_FOURCC_NV12;
    sys->params.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    sys->params.mfx.FrameInfo.Width         = QSV_ALIGN(16, enc->fmt_in.video.i_width);
    sys->params.mfx.FrameInfo.Height        = QSV_ALIGN(32, enc->fmt_in.video.i_height);
    sys->params.mfx.FrameInfo.CropW         = enc->fmt_in.video.i_visible_width;
    sys->params.mfx.FrameInfo.CropH         = enc->fmt_in.video.i_visible_height;
    sys->params.mfx.FrameInfo.PicStruct     = MFX_PICSTRUCT_UNKNOWN;
    /* Parsing options common to all RC methods and codecs */
    sys->params.IOPattern       = MFX_IOPATTERN_IN_SYSTEM_MEMORY;
    sys->params.AsyncDepth      = var_InheritInteger(enc, SOUT_CFG_PREFIX "async-depth");
    sys->params.mfx.GopPicSize  = var_InheritInteger(enc, SOUT_CFG_PREFIX "gop-size");
    sys->params.mfx.GopRefDist  = var_InheritInteger(enc, SOUT_CFG_PREFIX "gop-refdist");
    sys->params.mfx.IdrInterval = var_InheritInteger(enc, SOUT_CFG_PREFIX "idr-interval");
    sys->params.mfx.NumSlice    = var_InheritInteger(enc, SOUT_CFG_PREFIX "num-slice");
    sys->params.mfx.NumRefFrame = var_InheritInteger(enc, SOUT_CFG_PREFIX "num-ref-frame");
    sys->params.mfx.TargetUsage = qsv_params_get_value(target_usage_text,
        target_usage_list, sizeof(target_usage_list),
        var_InheritString(enc, SOUT_CFG_PREFIX "target-usage"));
    if (enc->fmt_out.i_codec == VLC_CODEC_H264) {
        sys->params.mfx.CodecId = MFX_CODEC_AVC;
        sys->params.mfx.CodecProfile = qsv_params_get_value(profile_h264_text,
            profile_h264_list, sizeof(profile_h264_list),
            var_InheritString(enc, SOUT_CFG_PREFIX "h264-profile"));
        sys->params.mfx.CodecLevel = qsv_params_get_value(level_h264_text,
            level_h264_list, sizeof(level_h264_list),
            var_InheritString(enc, SOUT_CFG_PREFIX "h264-level"));
        msg_Dbg(enc, "Encoder in H264 mode, with profile %d and level %d",
            sys->params.mfx.CodecProfile, sys->params.mfx.CodecLevel);
    } else {
        sys->params.mfx.CodecId = MFX_CODEC_MPEG2;
        sys->params.mfx.CodecProfile = qsv_params_get_value(profile_mpeg2_text,
            profile_mpeg2_list, sizeof(profile_mpeg2_list),
            var_InheritString(enc, SOUT_CFG_PREFIX "mpeg2-profile"));
        sys->params.mfx.CodecLevel = qsv_params_get_value(level_mpeg2_text,
            level_mpeg2_list, sizeof(level_mpeg2_list),
            var_InheritString(enc, SOUT_CFG_PREFIX "mpeg2-level"));
        msg_Dbg(enc, "Encoder in MPEG2 mode, with profile %d and level %d",
            sys->params.mfx.CodecProfile, sys->params.mfx.CodecLevel);
    }
    sys->params.mfx.RateControlMethod = qsv_params_get_value(rc_method_text,
        rc_method_list, sizeof(rc_method_list),
        var_InheritString(enc, SOUT_CFG_PREFIX "rc-method"));
    msg_Dbg(enc, "Encoder using '%s' Rate Control method",
        var_InheritString(enc, SOUT_CFG_PREFIX "rc-method"));
    if (sys->params.mfx.RateControlMethod == MFX_RATECONTROL_CQP) {
        sys->params.mfx.QPI = sys->params.mfx.QPB = sys->params.mfx.QPP =
            var_InheritInteger(enc, SOUT_CFG_PREFIX "qp");
        sys->params.mfx.QPI = var_InheritInteger(enc, SOUT_CFG_PREFIX "qpi");
        sys->params.mfx.QPB = var_InheritInteger(enc, SOUT_CFG_PREFIX "qpb");
        sys->params.mfx.QPP = var_InheritInteger(enc, SOUT_CFG_PREFIX "qpp");
    } else {
        if (!enc->fmt_out.i_bitrate) {
            msg_Warn(enc, "No bitrate specified, using default %d",
                QSV_BITRATE_DEFAULT);
            sys->params.mfx.TargetKbps = QSV_BITRATE_DEFAULT;
        } else
            sys->params.mfx.TargetKbps = enc->fmt_out.i_bitrate / 1000;
        if (sys->params.mfx.RateControlMethod == MFX_RATECONTROL_AVBR) {
            sys->params.mfx.Accuracy = var_InheritInteger(enc, SOUT_CFG_PREFIX "accuracy");
            sys->params.mfx.Convergence = var_InheritInteger(enc, SOUT_CFG_PREFIX "convergence");
        } else if (sys->params.mfx.RateControlMethod == MFX_RATECONTROL_VBR)
            sys->params.mfx.MaxKbps = var_InheritInteger(enc, SOUT_CFG_PREFIX "bitrate-max");
    }
    /* Initializing MFX_Encoder */
    sts = MFXVideoENCODE_Init(sys->session, &sys->params);
    if (sts == MFX_ERR_NONE)
        msg_Dbg(enc, "Successfuly initialized video encoder");
    else if (sts < MFX_ERR_NONE) {
        msg_Err(enc, "Unable to initialize video encoder error (%d). " \
            " Most likely because of provided encoding parameters", sts);
        goto error;
    } else
        msg_Warn(enc, "Video encoder initialization : %d. The stream might be corrupted/invalid", sts);
    /* Querying PPS/SPS Headers, BufferSizeInKB, ... */
    memset(&headers, 0, sizeof(headers));
    memset(&nals, 0, sizeof(nals));
    headers.Header.BufferId = MFX_EXTBUFF_CODING_OPTION_SPSPPS;
    headers.Header.BufferSz = sizeof(headers);
    headers.SPSBufSize      = headers.PPSBufSize = 64;
    headers.SPSBuffer       = nals;
    headers.PPSBuffer       = nals + 64;
    sys->params.ExtParam    = (mfxExtBuffer **)&extended_params;
    sys->params.NumExtParam = 1;
    MFXVideoENCODE_GetVideoParam(sys->session, &sys->params);
    sys->params.NumExtParam = 0;
    sys->params.ExtParam = NULL;
    i_extra = headers.SPSBufSize + headers.PPSBufSize;
    p_extra = malloc(i_extra);
    if (unlikely(!p_extra))
        goto nomem;
    memcpy(p_extra, headers.SPSBuffer, headers.SPSBufSize);
    memcpy(p_extra + headers.SPSBufSize, headers.PPSBuffer, headers.PPSBufSize);
    enc->fmt_out.p_extra = p_extra;
    enc->fmt_out.i_extra = i_extra;
    sys->async_depth = sys->params.AsyncDepth;
    sys->tasks = calloc(sys->async_depth, sizeof(async_task_t));
    if (unlikely(!sys->tasks))
        goto nomem;
    /* Request number of surface needed and creating frame pool */
    if (MFXVideoENCODE_QueryIOSurf(sys->session, &sys->params, &alloc_request)!= MFX_ERR_NONE)
        goto error;
    if (qsv_frame_pool_Init(&sys->frames, &alloc_request, sys->async_depth) != VLC_SUCCESS)
        goto nomem;
    msg_Dbg(enc, "Requested %d surfaces for work", alloc_request.NumFrameSuggested);
    enc->pf_encode_video = Encode;
    return VLC_SUCCESS;
 error:
    Close(this);
    return VLC_EGENERIC;
 nomem:
    Close(this);
    return VLC_ENOMEM;
}
