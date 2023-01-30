static av_cold int zero12v_decode_init(AVCodecContext *avctx)
{
    avctx->pix_fmt             = AV_PIX_FMT_YUV422P16;
    avctx->bits_per_raw_sample = 10;
    if (avctx->codec_tag == MKTAG('a', '1', '2', 'v'))
        avpriv_request_sample(avctx, "transparency");
    return 0;
}
