static int mpeg_decode_frame(AVCodecContext *avctx, void *data,
                             int *got_output, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int ret;
    int buf_size = avpkt->size;
    Mpeg1Context *s = avctx->priv_data;
    AVFrame *picture = data;
    MpegEncContext *s2 = &s->mpeg_enc_ctx;
    if (buf_size == 0 || (buf_size == 4 && AV_RB32(buf) == SEQ_END_CODE)) {
        
        if (s2->low_delay == 0 && s2->next_picture_ptr) {
            int ret = av_frame_ref(picture, s2->next_picture_ptr->f);
            if (ret < 0)
                return ret;
            s2->next_picture_ptr = NULL;
            *got_output = 1;
        }
        return buf_size;
    }
    if (s2->avctx->flags & AV_CODEC_FLAG_TRUNCATED) {
        int next = ff_mpeg1_find_frame_end(&s2->parse_context, buf,
                                           buf_size, NULL);
        if (ff_combine_frame(&s2->parse_context, next,
                             (const uint8_t **) &buf, &buf_size) < 0)
            return buf_size;
    }
    s2->codec_tag = avpriv_toupper4(avctx->codec_tag);
    if (s->mpeg_enc_ctx_allocated == 0 && (   s2->codec_tag == AV_RL32("VCR2")
                                           || s2->codec_tag == AV_RL32("BW10")
                                          ))
        vcr2_init_sequence(avctx);
    s->slice_count = 0;
    if (avctx->extradata && !s->extradata_decoded) {
        ret = decode_chunks(avctx, picture, got_output,
                            avctx->extradata, avctx->extradata_size);
        if (*got_output) {
            av_log(avctx, AV_LOG_ERROR, "picture in extradata\n");
            *got_output = 0;
        }
        s->extradata_decoded = 1;
        if (ret < 0 && (avctx->err_recognition & AV_EF_EXPLODE)) {
            s2->current_picture_ptr = NULL;
            return ret;
        }
    }
    ret = decode_chunks(avctx, picture, got_output, buf, buf_size);
    if (ret<0 || *got_output)
        s2->current_picture_ptr = NULL;
    return ret;
}
