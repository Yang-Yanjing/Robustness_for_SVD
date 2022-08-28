static void vaapi_decode_uninit(AVCodecContext *avctx)
{
    InputStream *ist = avctx->opaque;
    VAAPIDecoderContext *ctx = ist->hwaccel_ctx;
    if (ctx) {
        av_buffer_unref(&ctx->frames_ref);
        av_buffer_unref(&ctx->device_ref);
        av_free(ctx);
    }
    av_buffer_unref(&ist->hw_frames_ctx);
    ist->hwaccel_ctx           = NULL;
    ist->hwaccel_uninit        = NULL;
    ist->hwaccel_get_buffer    = NULL;
    ist->hwaccel_retrieve_data = NULL;
}
