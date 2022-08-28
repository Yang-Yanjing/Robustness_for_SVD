static int adx_encode_frame(AVCodecContext *avctx, AVPacket *avpkt,
                            const AVFrame *frame, int *got_packet_ptr)
{
    ADXContext *c          = avctx->priv_data;
    const int16_t *samples = (const int16_t *)frame->data[0];
    uint8_t *dst;
    int ch, out_size, ret;
    out_size = BLOCK_SIZE * avctx->channels + !c->header_parsed * HEADER_SIZE;
    if ((ret = ff_alloc_packet2(avctx, avpkt, out_size, 0)) < 0)
        return ret;
    dst = avpkt->data;
    if (!c->header_parsed) {
        int hdrsize;
        if ((hdrsize = adx_encode_header(avctx, dst, avpkt->size)) < 0) {
            av_log(avctx, AV_LOG_ERROR, "output buffer is too small\n");
            return AVERROR(EINVAL);
        }
        dst      += hdrsize;
        c->header_parsed = 1;
    }
    for (ch = 0; ch < avctx->channels; ch++) {
        adx_encode(c, dst, samples + ch, &c->prev[ch], avctx->channels);
        dst += BLOCK_SIZE;
    }
    *got_packet_ptr = 1;
    return 0;
}
