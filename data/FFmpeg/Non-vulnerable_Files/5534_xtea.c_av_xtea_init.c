void av_xtea_init(AVXTEA *ctx, const uint8_t key[16])
{
    int i;
    for (i = 0; i < 4; i++)
        ctx->key[i] = AV_RB32(key + (i << 2));
}
