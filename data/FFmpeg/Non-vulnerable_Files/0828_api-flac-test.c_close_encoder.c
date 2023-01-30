static int close_encoder(AVCodecContext **enc_ctx)
{
    avcodec_close(*enc_ctx);
    av_freep(enc_ctx);
    return 0;
}
