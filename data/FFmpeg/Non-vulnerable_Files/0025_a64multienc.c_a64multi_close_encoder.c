static av_cold int a64multi_close_encoder(AVCodecContext *avctx)
{
    A64Context *c = avctx->priv_data;
    av_freep(&c->mc_meta_charset);
    av_freep(&c->mc_best_cb);
    av_freep(&c->mc_charset);
    av_freep(&c->mc_charmap);
    av_freep(&c->mc_colram);
    return 0;
}
