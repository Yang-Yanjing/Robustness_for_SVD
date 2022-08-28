av_cold int ff_ac3_float_encode_init(AVCodecContext *avctx)
{
    AC3EncodeContext *s = avctx->priv_data;
    s->fdsp = avpriv_float_dsp_alloc(avctx->flags & AV_CODEC_FLAG_BITEXACT);
    if (!s->fdsp)
        return AVERROR(ENOMEM);
    return ff_ac3_encode_init(avctx);
}
