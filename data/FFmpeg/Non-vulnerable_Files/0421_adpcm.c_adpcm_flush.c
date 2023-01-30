static void adpcm_flush(AVCodecContext *avctx)
{
    ADPCMDecodeContext *c = avctx->priv_data;
    c->has_status = 0;
}
