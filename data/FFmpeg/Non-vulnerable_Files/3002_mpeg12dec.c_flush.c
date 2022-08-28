static void flush(AVCodecContext *avctx)
{
    Mpeg1Context *s = avctx->priv_data;
    s->sync       = 0;
    ff_mpeg_flush(avctx);
}
