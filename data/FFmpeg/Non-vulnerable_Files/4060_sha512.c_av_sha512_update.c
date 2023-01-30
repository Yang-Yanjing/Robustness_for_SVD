void av_sha512_update(AVSHA512* ctx, const uint8_t* data, unsigned int len)
{
    unsigned int i, j;
    j = ctx->count & 127;
    ctx->count += len;
#if CONFIG_SMALL
    for (i = 0; i < len; i++) {
        ctx->buffer[j++] = data[i];
        if (128 == j) {
            sha512_transform(ctx->state, ctx->buffer);
            j = 0;
        }
    }
#else
    if ((j + len) > 127) {
        memcpy(&ctx->buffer[j], data, (i = 128 - j));
        sha512_transform(ctx->state, ctx->buffer);
        for (; i + 127 < len; i += 128)
            sha512_transform(ctx->state, &data[i]);
        j = 0;
    } else
        i = 0;
    memcpy(&ctx->buffer[j], &data[i], len - i);
#endif
}
