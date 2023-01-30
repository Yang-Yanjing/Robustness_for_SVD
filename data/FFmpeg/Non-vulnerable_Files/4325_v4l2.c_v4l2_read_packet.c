static int v4l2_read_packet(AVFormatContext *ctx, AVPacket *pkt)
{
    struct video_data *s = ctx->priv_data;
#if FF_API_CODED_FRAME
FF_DISABLE_DEPRECATION_WARNINGS
    AVFrame *frame = ctx->streams[0]->codec->coded_frame;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    int res;
    av_init_packet(pkt);
    if ((res = mmap_read_frame(ctx, pkt)) < 0) {
        return res;
    }
#if FF_API_CODED_FRAME
FF_DISABLE_DEPRECATION_WARNINGS
    if (frame && s->interlaced) {
        frame->interlaced_frame = 1;
        frame->top_field_first = s->top_field_first;
    }
FF_ENABLE_DEPRECATION_WARNINGS
#endif
    return pkt->size;
}
