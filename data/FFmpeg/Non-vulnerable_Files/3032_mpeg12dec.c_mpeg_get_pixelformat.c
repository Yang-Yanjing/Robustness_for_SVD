static enum AVPixelFormat mpeg_get_pixelformat(AVCodecContext *avctx)
{
    Mpeg1Context *s1  = avctx->priv_data;
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    const enum AVPixelFormat *pix_fmts;
    if (CONFIG_GRAY && (avctx->flags & AV_CODEC_FLAG_GRAY))
        return AV_PIX_FMT_GRAY8;
    if (s->chroma_format < 2)
        pix_fmts = avctx->codec_id == AV_CODEC_ID_MPEG1VIDEO ?
                                mpeg1_hwaccel_pixfmt_list_420 :
                                mpeg2_hwaccel_pixfmt_list_420;
    else if (s->chroma_format == 2)
        pix_fmts = mpeg12_pixfmt_list_422;
    else
        pix_fmts = mpeg12_pixfmt_list_444;
    return ff_thread_get_format(avctx, pix_fmts);
}
