static av_cold int decode_init(AVCodecContext *avctx)
{
    FourXContext * const f = avctx->priv_data;
    int ret;
    if (avctx->extradata_size != 4 || !avctx->extradata) {
        av_log(avctx, AV_LOG_ERROR, "extradata wrong or missing\n");
        return AVERROR_INVALIDDATA;
    }
    if((avctx->width % 16) || (avctx->height % 16)) {
        av_log(avctx, AV_LOG_ERROR, "unsupported width/height\n");
        return AVERROR_INVALIDDATA;
    }
    ret = av_image_check_size(avctx->width, avctx->height, 0, avctx);
    if (ret < 0)
        return ret;
    f->frame_buffer      = av_mallocz(avctx->width * avctx->height * 2);
    f->last_frame_buffer = av_mallocz(avctx->width * avctx->height * 2);
    if (!f->frame_buffer || !f->last_frame_buffer) {
        decode_end(avctx);
        return AVERROR(ENOMEM);
    }
    f->version = AV_RL32(avctx->extradata) >> 16;
    ff_blockdsp_init(&f->bdsp, avctx);
    ff_bswapdsp_init(&f->bbdsp);
    f->avctx = avctx;
    init_vlcs(f);
    if (f->version > 2)
        avctx->pix_fmt = AV_PIX_FMT_RGB565;
    else
        avctx->pix_fmt = AV_PIX_FMT_BGR555;
    return 0;
}
