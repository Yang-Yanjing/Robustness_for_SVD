static int vfw_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    struct vfw_ctx *ctx = s->priv_data;
    AVPacketList *pktl = NULL;
    while(!pktl) {
        WaitForSingleObject(ctx->mutex, INFINITE);
        pktl = ctx->pktl;
        if(ctx->pktl) {
            *pkt = ctx->pktl->pkt;
            ctx->pktl = ctx->pktl->next;
            av_free(pktl);
        }
        ResetEvent(ctx->event);
        ReleaseMutex(ctx->mutex);
        if(!pktl) {
            if(s->flags & AVFMT_FLAG_NONBLOCK) {
                return AVERROR(EAGAIN);
            } else {
                WaitForSingleObject(ctx->event, INFINITE);
            }
        }
    }
    ctx->curbufsize -= pkt->size;
    return pkt->size;
}
