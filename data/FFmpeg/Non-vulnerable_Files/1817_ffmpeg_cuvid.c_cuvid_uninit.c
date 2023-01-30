static void cuvid_uninit(AVCodecContext *avctx)
{
    InputStream *ist = avctx->opaque;
    av_buffer_unref(&ist->hw_frames_ctx);
}
