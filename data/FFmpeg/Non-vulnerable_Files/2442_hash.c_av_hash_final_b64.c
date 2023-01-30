void av_hash_final_b64(struct AVHashContext *ctx, uint8_t *dst, int size)
{
    uint8_t buf[AV_HASH_MAX_SIZE], b64[AV_BASE64_SIZE(AV_HASH_MAX_SIZE)];
    unsigned rsize = av_hash_get_size(ctx), osize;
    av_hash_final(ctx, buf);
    av_base64_encode(b64, sizeof(b64), buf, rsize);
    osize = AV_BASE64_SIZE(rsize);
    memcpy(dst, b64, FFMIN(osize, size));
    if (size < osize)
        dst[size - 1] = 0;
}
