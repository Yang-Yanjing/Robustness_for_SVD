int qsv_init(AVCodecContext *s)
{
    InputStream *ist = s->opaque;
    AVHWFramesContext *frames_ctx;
    AVQSVFramesContext *frames_hwctx;
    int ret;
    if (!hw_device_ctx) {
        ret = qsv_device_init(ist);
        if (ret < 0)
            return ret;
    }
    av_buffer_unref(&ist->hw_frames_ctx);
    ist->hw_frames_ctx = av_hwframe_ctx_alloc(hw_device_ctx);
    if (!ist->hw_frames_ctx)
        return AVERROR(ENOMEM);
    frames_ctx   = (AVHWFramesContext*)ist->hw_frames_ctx->data;
    frames_hwctx = frames_ctx->hwctx;
    frames_ctx->width             = FFALIGN(s->coded_width,  32);
    frames_ctx->height            = FFALIGN(s->coded_height, 32);
    frames_ctx->format            = AV_PIX_FMT_QSV;
    frames_ctx->sw_format         = s->sw_pix_fmt;
    frames_ctx->initial_pool_size = 64;
    frames_hwctx->frame_type      = MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET;
    ret = av_hwframe_ctx_init(ist->hw_frames_ctx);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error initializing a QSV frame pool\n");
        return ret;
    }
    ist->hwaccel_get_buffer = qsv_get_buffer;
    ist->hwaccel_uninit     = qsv_uninit;
    return 0;
}
