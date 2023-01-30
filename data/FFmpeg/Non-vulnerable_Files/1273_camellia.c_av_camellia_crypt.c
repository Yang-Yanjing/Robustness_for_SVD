void av_camellia_crypt(AVCAMELLIA *cs, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt)
{
    int i;
    while (count--) {
        if (decrypt) {
            camellia_decrypt(cs, dst, src, iv);
        } else {
            if (iv) {
                for (i = 0; i < 16; i++)
                    dst[i] = src[i] ^ iv[i];
                camellia_encrypt(cs, dst, dst);
                memcpy(iv, dst, 16);
            } else {
                camellia_encrypt(cs, dst, src);
            }
        }
        src = src + 16;
        dst = dst + 16;
    }
}
