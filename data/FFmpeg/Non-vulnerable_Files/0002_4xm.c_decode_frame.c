static int decode_frame(AVCodecContext *avctx, void *data,
                        int *got_frame, AVPacket *avpkt)
{
    const uint8_t *buf    = avpkt->data;
    int buf_size          = avpkt->size;
    FourXContext *const f = avctx->priv_data;
    AVFrame *picture      = data;
    int i, frame_4cc, frame_size, ret;
    if (buf_size < 20)
        return AVERROR_INVALIDDATA;
    av_assert0(avctx->width % 16 == 0 && avctx->height % 16 == 0);
    if (buf_size < AV_RL32(buf + 4) + 8) {
        av_log(f->avctx, AV_LOG_ERROR, "size mismatch %d %"PRIu32"\n",
               buf_size, AV_RL32(buf + 4));
        return AVERROR_INVALIDDATA;
    }
    frame_4cc = AV_RL32(buf);
    if (frame_4cc == AV_RL32("cfrm")) {
        int free_index       = -1;
        int id, whole_size;
        const int data_size  = buf_size - 20;
        CFrameBuffer *cfrm;
        if (f->version <= 1) {
            av_log(f->avctx, AV_LOG_ERROR, "cfrm in version %d\n", f->version);
            return AVERROR_INVALIDDATA;
        }
        id         = AV_RL32(buf + 12);
        whole_size = AV_RL32(buf + 16);
        if (data_size < 0 || whole_size < 0) {
            av_log(f->avctx, AV_LOG_ERROR, "sizes invalid\n");
            return AVERROR_INVALIDDATA;
        }
        for (i = 0; i < CFRAME_BUFFER_COUNT; i++)
            if (f->cfrm[i].id && f->cfrm[i].id < avctx->frame_number)
                av_log(f->avctx, AV_LOG_ERROR, "lost c frame %d\n",
                       f->cfrm[i].id);
        for (i = 0; i < CFRAME_BUFFER_COUNT; i++) {
            if (f->cfrm[i].id == id)
                break;
            if (f->cfrm[i].size == 0)
                free_index = i;
        }
        if (i >= CFRAME_BUFFER_COUNT) {
            i             = free_index;
            f->cfrm[i].id = id;
        }
        cfrm = &f->cfrm[i];
        if (data_size > UINT_MAX -  cfrm->size - AV_INPUT_BUFFER_PADDING_SIZE)
            return AVERROR_INVALIDDATA;
        cfrm->data = av_fast_realloc(cfrm->data, &cfrm->allocated_size,
                                     cfrm->size + data_size + AV_INPUT_BUFFER_PADDING_SIZE);
        
        if (!cfrm->data) {
            av_log(f->avctx, AV_LOG_ERROR, "realloc failure\n");
            return AVERROR(ENOMEM);
        }
        memcpy(cfrm->data + cfrm->size, buf + 20, data_size);
        cfrm->size += data_size;
        if (cfrm->size >= whole_size) {
            buf        = cfrm->data;
            frame_size = cfrm->size;
            if (id != avctx->frame_number)
                av_log(f->avctx, AV_LOG_ERROR, "cframe id mismatch %d %d\n",
                       id, avctx->frame_number);
            if (f->version <= 1)
                return AVERROR_INVALIDDATA;
            cfrm->size = cfrm->id = 0;
            frame_4cc  = AV_RL32("pfrm");
        } else
            return buf_size;
    } else {
        buf        = buf      + 12;
        frame_size = buf_size - 12;
    }
    if ((ret = ff_get_buffer(avctx, picture, 0)) < 0)
        return ret;
    if (frame_4cc == AV_RL32("ifr2")) {
        picture->pict_type = AV_PICTURE_TYPE_I;
        if ((ret = decode_i2_frame(f, buf - 4, frame_size + 4)) < 0) {
            av_log(f->avctx, AV_LOG_ERROR, "decode i2 frame failed\n");
            return ret;
        }
    } else if (frame_4cc == AV_RL32("ifrm")) {
        picture->pict_type = AV_PICTURE_TYPE_I;
        if ((ret = decode_i_frame(f, buf, frame_size)) < 0) {
            av_log(f->avctx, AV_LOG_ERROR, "decode i frame failed\n");
            return ret;
        }
    } else if (frame_4cc == AV_RL32("pfrm") || frame_4cc == AV_RL32("pfr2")) {
        picture->pict_type = AV_PICTURE_TYPE_P;
        if ((ret = decode_p_frame(f, buf, frame_size)) < 0) {
            av_log(f->avctx, AV_LOG_ERROR, "decode p frame failed\n");
            return ret;
        }
    } else if (frame_4cc == AV_RL32("snd_")) {
        av_log(avctx, AV_LOG_ERROR, "ignoring snd_ chunk length:%d\n",
               buf_size);
    } else {
        av_log(avctx, AV_LOG_ERROR, "ignoring unknown chunk length:%d\n",
               buf_size);
    }
    picture->key_frame = picture->pict_type == AV_PICTURE_TYPE_I;
    av_image_copy_plane(picture->data[0], picture->linesize[0],
                        (const uint8_t*)f->frame_buffer,  avctx->width * 2,
                        avctx->width * 2, avctx->height);
    FFSWAP(uint16_t *, f->frame_buffer, f->last_frame_buffer);
    *got_frame = 1;
    emms_c();
    return buf_size;
}