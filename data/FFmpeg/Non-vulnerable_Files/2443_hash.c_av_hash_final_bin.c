void av_hash_final_bin(struct AVHashContext *ctx, uint8_t *dst, int size)
{
    uint8_t buf[AV_HASH_MAX_SIZE];
    unsigned rsize = av_hash_get_size(ctx);
    av_hash_final(ctx, buf);
    memcpy(dst, buf, FFMIN(size, rsize));
    if (size > rsize)
        memset(dst + rsize, 0, size - rsize);
}
