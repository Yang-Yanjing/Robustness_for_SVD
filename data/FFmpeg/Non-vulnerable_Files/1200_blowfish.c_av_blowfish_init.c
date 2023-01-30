av_cold void av_blowfish_init(AVBlowfish *ctx, const uint8_t *key, int key_len)
{
    uint32_t data, data_l, data_r;
    int i, j, k;
    memcpy(ctx->s, orig_s, sizeof(orig_s));
    j = 0;
    for (i = 0; i < AV_BF_ROUNDS + 2; ++i) {
        data = 0;
        for (k = 0; k < 4; k++) {
            data = (data << 8) | key[j];
            if (++j >= key_len)
                j = 0;
        }
        ctx->p[i] = orig_p[i] ^ data;
    }
    data_l = data_r = 0;
    for (i = 0; i < AV_BF_ROUNDS + 2; i += 2) {
        av_blowfish_crypt_ecb(ctx, &data_l, &data_r, 0);
        ctx->p[i]     = data_l;
        ctx->p[i + 1] = data_r;
    }
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 256; j += 2) {
            av_blowfish_crypt_ecb(ctx, &data_l, &data_r, 0);
            ctx->s[i][j]     = data_l;
            ctx->s[i][j + 1] = data_r;
        }
    }
}
