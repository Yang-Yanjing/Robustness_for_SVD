static av_cold int aasc_decode_end(AVCodecContext *avctx)
{
    AascContext *s = avctx->priv_data;
    av_frame_free(&s->frame);
    return 0;
}
