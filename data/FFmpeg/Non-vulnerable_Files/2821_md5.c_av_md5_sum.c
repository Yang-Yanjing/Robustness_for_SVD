void av_md5_sum(uint8_t *dst, const uint8_t *src, const int len)
{
    AVMD5 ctx;
    av_md5_init(&ctx);
    av_md5_update(&ctx, src, len);
    av_md5_final(&ctx, dst);
}
