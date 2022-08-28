static void decoder_destroy(Decoder *d) {
    av_packet_unref(&d->pkt);
    avcodec_free_context(&d->avctx);
}
