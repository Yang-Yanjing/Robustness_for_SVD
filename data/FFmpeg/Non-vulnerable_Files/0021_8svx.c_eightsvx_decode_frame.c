static int eightsvx_decode_frame(AVCodecContext *avctx, void *data,
                                 int *got_frame_ptr, AVPacket *avpkt)
{
    EightSvxContext *esc = avctx->priv_data;
    AVFrame *frame       = data;
    int buf_size;
    int ch, ret;
    int hdr_size = 2;
    
    if (!esc->data[0] && avpkt) {
        int chan_size = avpkt->size / avctx->channels - hdr_size;
        if (avpkt->size % avctx->channels) {
            av_log(avctx, AV_LOG_WARNING, "Packet with odd size, ignoring last byte\n");
        }
        if (avpkt->size < (hdr_size + 1) * avctx->channels) {
            av_log(avctx, AV_LOG_ERROR, "packet size is too small\n");
            return AVERROR_INVALIDDATA;
        }
        esc->fib_acc[0] = avpkt->data[1] + 128;
        if (avctx->channels == 2)
            esc->fib_acc[1] = avpkt->data[2+chan_size+1] + 128;
        esc->data_idx  = 0;
        esc->data_size = chan_size;
        if (!(esc->data[0] = av_malloc(chan_size)))
            return AVERROR(ENOMEM);
        if (avctx->channels == 2) {
            if (!(esc->data[1] = av_malloc(chan_size))) {
                av_freep(&esc->data[0]);
                return AVERROR(ENOMEM);
            }
        }
        memcpy(esc->data[0], &avpkt->data[hdr_size], chan_size);
        if (avctx->channels == 2)
            memcpy(esc->data[1], &avpkt->data[2*hdr_size+chan_size], chan_size);
    }
    if (!esc->data[0]) {
        av_log(avctx, AV_LOG_ERROR, "unexpected empty packet\n");
        return AVERROR_INVALIDDATA;
    }
    
    buf_size = FFMIN(MAX_FRAME_SIZE, esc->data_size - esc->data_idx);
    if (buf_size <= 0) {
        *got_frame_ptr = 0;
        return avpkt->size;
    }
    
    frame->nb_samples = buf_size * 2;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;
    for (ch = 0; ch < avctx->channels; ch++) {
        delta_decode(frame->data[ch], &esc->data[ch][esc->data_idx],
                     buf_size, &esc->fib_acc[ch], esc->table);
    }
    esc->data_idx += buf_size;
    *got_frame_ptr = 1;
    return ((avctx->frame_number == 0)*hdr_size + buf_size)*avctx->channels;
}
