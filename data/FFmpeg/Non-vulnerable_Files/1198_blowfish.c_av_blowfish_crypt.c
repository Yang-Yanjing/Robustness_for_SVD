void av_blowfish_crypt(AVBlowfish *ctx, uint8_t *dst, const uint8_t *src,
                       int count, uint8_t *iv, int decrypt)
{
    uint32_t v0, v1;
    int i;
    if (decrypt) {
        while (count--) {
            v0 = AV_RB32(src);
            v1 = AV_RB32(src + 4);
            av_blowfish_crypt_ecb(ctx, &v0, &v1, decrypt);
            if (iv) {
                v0 ^= AV_RB32(iv);
                v1 ^= AV_RB32(iv + 4);
                memcpy(iv, src, 8);
            }
            AV_WB32(dst, v0);
            AV_WB32(dst + 4, v1);
            src   += 8;
            dst   += 8;
        }
    } else {
        while (count--) {
            if (iv) {
                for (i = 0; i < 8; i++)
                    dst[i] = src[i] ^ iv[i];
                v0 = AV_RB32(dst);
                v1 = AV_RB32(dst + 4);
            } else {
                v0 = AV_RB32(src);
                v1 = AV_RB32(src + 4);
            }
            av_blowfish_crypt_ecb(ctx, &v0, &v1, decrypt);
            AV_WB32(dst, v0);
            AV_WB32(dst + 4, v1);
            if (iv)
                memcpy(iv, dst, 8);
            src   += 8;
            dst   += 8;
        }
    }
}
