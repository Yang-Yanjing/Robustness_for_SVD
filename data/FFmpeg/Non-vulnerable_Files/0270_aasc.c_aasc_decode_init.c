static av_cold int aasc_decode_init(AVCodecContext *avctx)
{
    AascContext *s = avctx->priv_data;
    uint8_t *ptr;
    int i;
    s->avctx = avctx;
    switch (avctx->bits_per_coded_sample) {
    case 8:
        avctx->pix_fmt = AV_PIX_FMT_PAL8;
        ptr = avctx->extradata;
        s->palette_size = FFMIN(avctx->extradata_size, AVPALETTE_SIZE);
        for (i = 0; i < s->palette_size / 4; i++) {
            s->palette[i] = 0xFFU << 24 | AV_RL32(ptr);
            ptr += 4;
        }
        break;
    case 16:
        avctx->pix_fmt = AV_PIX_FMT_RGB555LE;
        break;
    case 24:
        avctx->pix_fmt = AV_PIX_FMT_BGR24;
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unsupported bit depth: %d\n", avctx->bits_per_coded_sample);
        return -1;
    }
    s->frame = av_frame_alloc();
    if (!s->frame)
        return AVERROR(ENOMEM);
    return 0;
}
