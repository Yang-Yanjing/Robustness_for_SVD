static int vdpau_alloc(AVCodecContext *s)
{
    InputStream  *ist = s->opaque;
    int loglevel = (ist->hwaccel_id == HWACCEL_AUTO) ? AV_LOG_VERBOSE : AV_LOG_ERROR;
    VDPAUContext *ctx;
    int ret;
    AVBufferRef          *device_ref = NULL;
    AVHWDeviceContext    *device_ctx;
    AVVDPAUDeviceContext *device_hwctx;
    AVHWFramesContext    *frames_ctx;
    ctx = av_mallocz(sizeof(*ctx));
    if (!ctx)
        return AVERROR(ENOMEM);
    ist->hwaccel_ctx           = ctx;
    ist->hwaccel_uninit        = vdpau_uninit;
    ist->hwaccel_get_buffer    = vdpau_get_buffer;
    ist->hwaccel_retrieve_data = vdpau_retrieve_data;
    ctx->tmp_frame = av_frame_alloc();
    if (!ctx->tmp_frame)
        goto fail;
    ret = av_hwdevice_ctx_create(&device_ref, AV_HWDEVICE_TYPE_VDPAU,
                                 ist->hwaccel_device, NULL, 0);
    if (ret < 0)
        goto fail;
    device_ctx   = (AVHWDeviceContext*)device_ref->data;
    device_hwctx = device_ctx->hwctx;
    ctx->hw_frames_ctx = av_hwframe_ctx_alloc(device_ref);
    if (!ctx->hw_frames_ctx)
        goto fail;
    av_buffer_unref(&device_ref);
    frames_ctx            = (AVHWFramesContext*)ctx->hw_frames_ctx->data;
    frames_ctx->format    = AV_PIX_FMT_VDPAU;
    frames_ctx->sw_format = s->sw_pix_fmt;
    frames_ctx->width     = s->coded_width;
    frames_ctx->height    = s->coded_height;
    ret = av_hwframe_ctx_init(ctx->hw_frames_ctx);
    if (ret < 0)
        goto fail;
    if (av_vdpau_bind_context(s, device_hwctx->device, device_hwctx->get_proc_address, 0))
        goto fail;
    av_log(NULL, AV_LOG_VERBOSE, "Using VDPAU to decode input stream #%d:%d.\n",
           ist->file_index, ist->st->index);
    return 0;
fail:
    av_log(NULL, loglevel, "VDPAU init failed for stream #%d:%d.\n",
           ist->file_index, ist->st->index);
    av_buffer_unref(&device_ref);
    vdpau_uninit(s);
    return AVERROR(EINVAL);
}
