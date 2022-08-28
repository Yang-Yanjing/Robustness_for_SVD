
static int decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt)
{
    int ret;
    *got_frame = 0;
    if (pkt) {
        ret = avcodec_send_packet(avctx, pkt);
        
        
        if (ret < 0 && ret != AVERROR_EOF)
            return ret;
    }
    ret = avcodec_receive_frame(avctx, frame);
    if (ret < 0 && ret != AVERROR(EAGAIN))
        return ret;
    if (ret >= 0)
        *got_frame = 1;
    return 0;
}
