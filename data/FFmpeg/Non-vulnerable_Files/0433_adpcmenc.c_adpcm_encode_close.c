static av_cold int adpcm_encode_close(AVCodecContext *avctx)
{
    ADPCMEncodeContext *s = avctx->priv_data;
    av_freep(&s->paths);
    av_freep(&s->node_buf);
    av_freep(&s->nodep_buf);
    av_freep(&s->trellis_hash);
    return 0;
}
