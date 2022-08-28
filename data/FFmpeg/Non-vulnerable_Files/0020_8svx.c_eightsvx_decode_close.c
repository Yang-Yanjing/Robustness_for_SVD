static av_cold int eightsvx_decode_close(AVCodecContext *avctx)
{
    EightSvxContext *esc = avctx->priv_data;
    av_freep(&esc->data[0]);
    av_freep(&esc->data[1]);
    esc->data_size = 0;
    esc->data_idx = 0;
    return 0;
}
