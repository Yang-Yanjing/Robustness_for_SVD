void av_tea_crypt(AVTEA *ctx, uint8_t *dst, const uint8_t *src, int count,
                  uint8_t *iv, int decrypt)
{
    int i;
    if (decrypt) {
        while (count--) {
            tea_crypt_ecb(ctx, dst, src, decrypt, iv);
            src   += 8;
            dst   += 8;
        }
    } else {
        while (count--) {
            if (iv) {
                for (i = 0; i < 8; i++)
                    dst[i] = src[i] ^ iv[i];
                tea_crypt_ecb(ctx, dst, dst, decrypt, NULL);
                memcpy(iv, dst, 8);
            } else {
                tea_crypt_ecb(ctx, dst, src, decrypt, NULL);
            }
            src   += 8;
            dst   += 8;
        }
    }
}
