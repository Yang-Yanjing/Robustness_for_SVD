static void adx_decode_flush(AVCodecContext *avctx)
{
    ADXContext *c = avctx->priv_data;
    memset(c->prev, 0, sizeof(c->prev));
    c->eof = 0;
}
