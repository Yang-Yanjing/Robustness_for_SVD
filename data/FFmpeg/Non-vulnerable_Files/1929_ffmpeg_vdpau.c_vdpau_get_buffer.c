static int vdpau_get_buffer(AVCodecContext *s, AVFrame *frame, int flags)
{
    InputStream         *ist = s->opaque;
    VDPAUContext        *ctx = ist->hwaccel_ctx;
    return av_hwframe_get_buffer(ctx->hw_frames_ctx, frame, 0);
}
