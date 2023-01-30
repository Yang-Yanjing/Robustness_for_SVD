int vaapi_decode_init(AVCodecContext *avctx)
{
    InputStream *ist = avctx->opaque;
    VAAPIDecoderContext *ctx;
    int err;
    int loglevel = (ist->hwaccel_id != HWACCEL_VAAPI ? AV_LOG_VERBOSE
                                                     : AV_LOG_ERROR);
    if (ist->hwaccel_ctx)
        vaapi_decode_uninit(avctx);
    
    
    
    if (!hw_device_ctx) {
        err = vaapi_device_init(ist->hwaccel_device);
        if (err < 0)
            return err;
    }
    ctx = av_mallocz(sizeof(*ctx));
    if (!ctx)
        return AVERROR(ENOMEM);
    ctx->class = &vaapi_class;
    ist->hwaccel_ctx = ctx;
    ctx->device_ref = av_buffer_ref(hw_device_ctx);
    ctx->device = (AVHWDeviceContext*)ctx->device_ref->data;
    ctx->output_format = ist->hwaccel_output_format;
    avctx->pix_fmt = ctx->output_format;
    ctx->frames_ref = av_hwframe_ctx_alloc(ctx->device_ref);
    if (!ctx->frames_ref) {
        av_log(ctx, loglevel, "Failed to create VAAPI frame context.\n");
        err = AVERROR(ENOMEM);
        goto fail;
    }
    ctx->frames = (AVHWFramesContext*)ctx->frames_ref->data;
    ctx->frames->format = AV_PIX_FMT_VAAPI;
    ctx->frames->width  = avctx->coded_width;
    ctx->frames->height = avctx->coded_height;
    
    
    
    ctx->frames->sw_format = (avctx->sw_pix_fmt == AV_PIX_FMT_YUV420P10 ?
                              AV_PIX_FMT_P010 : AV_PIX_FMT_NV12);
    
    
    ctx->frames->initial_pool_size = DEFAULT_SURFACES;
    if (avctx->active_thread_type & FF_THREAD_FRAME)
        ctx->frames->initial_pool_size += avctx->thread_count;
    err = av_hwframe_ctx_init(ctx->frames_ref);
    if (err < 0) {
        av_log(ctx, loglevel, "Failed to initialise VAAPI frame "
               "context: %d\n", err);
        goto fail;
    }
    ist->hw_frames_ctx = av_buffer_ref(ctx->frames_ref);
    if (!ist->hw_frames_ctx) {
        err = AVERROR(ENOMEM);
        goto fail;
    }
    ist->hwaccel_uninit        = &vaapi_decode_uninit;
    ist->hwaccel_get_buffer    = &vaapi_get_buffer;
    ist->hwaccel_retrieve_data = &vaapi_retrieve_data;
    return 0;
fail:
    vaapi_decode_uninit(avctx);
    return err;
}
