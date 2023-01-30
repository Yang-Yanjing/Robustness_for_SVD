static void vdpau_uninit(AVCodecContext *s)
{
    InputStream  *ist = s->opaque;
    VDPAUContext *ctx = ist->hwaccel_ctx;
    ist->hwaccel_uninit        = NULL;
    ist->hwaccel_get_buffer    = NULL;
    ist->hwaccel_retrieve_data = NULL;
    av_buffer_unref(&ctx->hw_frames_ctx);
    av_frame_free(&ctx->tmp_frame);
    av_freep(&ist->hwaccel_ctx);
    av_freep(&s->hwaccel_context);
}
