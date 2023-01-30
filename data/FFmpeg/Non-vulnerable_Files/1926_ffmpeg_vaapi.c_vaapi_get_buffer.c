static int vaapi_get_buffer(AVCodecContext *avctx, AVFrame *frame, int flags)
{
    InputStream *ist = avctx->opaque;
    VAAPIDecoderContext *ctx = ist->hwaccel_ctx;
    int err;
    err = av_hwframe_get_buffer(ctx->frames_ref, frame, 0);
    if (err < 0) {
        av_log(ctx, AV_LOG_ERROR, "Failed to allocate decoder surface.\n");
    } else {
        av_log(ctx, AV_LOG_DEBUG, "Decoder given surface %#x.\n",
               (unsigned int)(uintptr_t)frame->data[3]);
    }
    return err;
}
