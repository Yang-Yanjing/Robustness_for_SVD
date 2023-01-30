static int aasc_decode_frame(AVCodecContext *avctx,
                              void *data, int *got_frame,
                              AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size       = avpkt->size;
    AascContext *s     = avctx->priv_data;
    int compr, i, stride, psize, ret;
    if (buf_size < 4) {
        av_log(avctx, AV_LOG_ERROR, "frame too short\n");
        return AVERROR_INVALIDDATA;
    }
    if ((ret = ff_reget_buffer(avctx, s->frame)) < 0)
        return ret;
    compr     = AV_RL32(buf);
    buf      += 4;
    buf_size -= 4;
    psize = avctx->bits_per_coded_sample / 8;
    switch (avctx->codec_tag) {
    case MKTAG('A', 'A', 'S', '4'):
        bytestream2_init(&s->gb, buf - 4, buf_size + 4);
        ff_msrle_decode(avctx, (AVPicture*)s->frame, 8, &s->gb);
        break;
    case MKTAG('A', 'A', 'S', 'C'):
    switch (compr) {
    case 0:
        stride = (avctx->width * psize + psize) & ~psize;
        if (buf_size < stride * avctx->height)
            return AVERROR_INVALIDDATA;
        for (i = avctx->height - 1; i >= 0; i--) {
            memcpy(s->frame->data[0] + i * s->frame->linesize[0], buf, avctx->width * psize);
            buf += stride;
            buf_size -= stride;
        }
        break;
    case 1:
        bytestream2_init(&s->gb, buf, buf_size);
        ff_msrle_decode(avctx, (AVPicture*)s->frame, 8, &s->gb);
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unknown compression type %d\n", compr);
        return AVERROR_INVALIDDATA;
    }
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unknown FourCC: %X\n", avctx->codec_tag);
        return -1;
    }
    if (avctx->pix_fmt == AV_PIX_FMT_PAL8)
        memcpy(s->frame->data[1], s->palette, s->palette_size);
    *got_frame = 1;
    if ((ret = av_frame_ref(data, s->frame)) < 0)
        return ret;
    
    return avpkt->size;
}
