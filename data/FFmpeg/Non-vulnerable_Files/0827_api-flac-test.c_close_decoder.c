static int close_decoder(AVCodecContext **dec_ctx)
{
    avcodec_close(*dec_ctx);
    av_freep(dec_ctx);
    return 0;
}
