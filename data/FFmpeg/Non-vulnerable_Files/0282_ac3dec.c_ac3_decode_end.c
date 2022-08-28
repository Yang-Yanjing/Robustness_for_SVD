static av_cold int ac3_decode_end(AVCodecContext *avctx)
{
    AC3DecodeContext *s = avctx->priv_data;
    ff_mdct_end(&s->imdct_512);
    ff_mdct_end(&s->imdct_256);
    av_freep(&s->fdsp);
    return 0;
}
