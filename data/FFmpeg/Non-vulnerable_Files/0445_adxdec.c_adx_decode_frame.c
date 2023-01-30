static int adx_decode_frame(AVCodecContext *avctx, void *data,
                            int *got_frame_ptr, AVPacket *avpkt)
{
    AVFrame *frame      = data;
    int buf_size        = avpkt->size;
    ADXContext *c       = avctx->priv_data;
    int16_t **samples;
    int samples_offset;
    const uint8_t *buf  = avpkt->data;
    const uint8_t *buf_end = buf + avpkt->size;
    int num_blocks, ch, ret;
    if (c->eof) {
        *got_frame_ptr = 0;
        return buf_size;
    }
    if (!c->header_parsed && buf_size >= 2 && AV_RB16(buf) == 0x8000) {
        int header_size;
        if ((ret = ff_adx_decode_header(avctx, buf, buf_size, &header_size,
                                        c->coeff)) < 0) {
            av_log(avctx, AV_LOG_ERROR, "error parsing ADX header\n");
            return AVERROR_INVALIDDATA;
        }
        c->channels      = avctx->channels;
        c->header_parsed = 1;
        if (buf_size < header_size)
            return AVERROR_INVALIDDATA;
        buf      += header_size;
        buf_size -= header_size;
    }
    if (!c->header_parsed)
        return AVERROR_INVALIDDATA;
    
    num_blocks = buf_size / (BLOCK_SIZE * c->channels);
    

    if (!num_blocks || buf_size % (BLOCK_SIZE * avctx->channels)) {
        if (buf_size >= 4 && (AV_RB16(buf) & 0x8000)) {
            c->eof = 1;
            *got_frame_ptr = 0;
            return avpkt->size;
        }
        return AVERROR_INVALIDDATA;
    }
    
    frame->nb_samples = num_blocks * BLOCK_SAMPLES;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;
    samples = (int16_t **)frame->extended_data;
    samples_offset = 0;
    while (num_blocks--) {
        for (ch = 0; ch < c->channels; ch++) {
            if (buf_end - buf < BLOCK_SIZE || adx_decode(c, samples[ch], samples_offset, buf, ch)) {
                c->eof = 1;
                buf = avpkt->data + avpkt->size;
                break;
            }
            buf_size -= BLOCK_SIZE;
            buf      += BLOCK_SIZE;
        }
        if (!c->eof)
            samples_offset += BLOCK_SAMPLES;
    }
    frame->nb_samples = samples_offset;
    *got_frame_ptr = 1;
    return buf - avpkt->data;
}
