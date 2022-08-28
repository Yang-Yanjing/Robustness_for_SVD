static int decode_frame(AVCodecContext *avctx, void *data,
                        int *got_frame, AVPacket *avpkt)
{
    AVFrame *frame = data;
    const uint8_t *buf = avpkt->data;
    int buf_size       = avpkt->size;
    EightBpsContext * const c = avctx->priv_data;
    const unsigned char *encoded = buf;
    unsigned char *pixptr, *pixptr_end;
    unsigned int height = avctx->height; 
    unsigned int dlen, p, row;
    const unsigned char *lp, *dp, *ep;
    unsigned char count;
    unsigned int planes     = c->planes;
    unsigned char *planemap = c->planemap;
    int ret;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;
    ep = encoded + buf_size;
    
    dp = encoded + planes * (height << 1);
    for (p = 0; p < planes; p++) {
        
        lp = encoded + p * (height << 1);
        
        for (row = 0; row < height; row++) {
            pixptr = frame->data[0] + row * frame->linesize[0] + planemap[p];
            pixptr_end = pixptr + frame->linesize[0];
            if (ep - lp < row * 2 + 2)
                return AVERROR_INVALIDDATA;
            dlen = av_be2ne16(*(const unsigned short *)(lp + row * 2));
            
            while (dlen > 0) {
                if (ep - dp <= 1)
                    return AVERROR_INVALIDDATA;
                if ((count = *dp++) <= 127) {
                    count++;
                    dlen -= count + 1;
                    if (pixptr_end - pixptr < count * planes)
                        break;
                    if (ep - dp < count)
                        return AVERROR_INVALIDDATA;
                    while (count--) {
                        *pixptr = *dp++;
                        pixptr += planes;
                    }
                } else {
                    count = 257 - count;
                    if (pixptr_end - pixptr < count * planes)
                        break;
                    while (count--) {
                        *pixptr = *dp;
                        pixptr += planes;
                    }
                    dp++;
                    dlen -= 2;
                }
            }
        }
    }
    if (avctx->bits_per_coded_sample <= 8) {
        const uint8_t *pal = av_packet_get_side_data(avpkt,
                                                     AV_PKT_DATA_PALETTE,
                                                     NULL);
        if (pal) {
            frame->palette_has_changed = 1;
            memcpy(c->pal, pal, AVPALETTE_SIZE);
        }
        memcpy (frame->data[1], c->pal, AVPALETTE_SIZE);
    }
    *got_frame = 1;
    
    return buf_size;
}
