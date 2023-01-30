void av_cast5_crypt2(AVCAST5* cs, uint8_t* dst, const uint8_t* src, int count, uint8_t *iv, int decrypt)
{
    int i;
    while (count--) {
        if (decrypt) {
            decipher(cs, dst, src, iv);
        } else {
            if (iv) {
                for (i = 0; i < 8; i++)
                    dst[i] = src[i] ^ iv[i];
                encipher(cs, dst, dst);
                memcpy(iv, dst, 8);
            } else {
                encipher(cs, dst, src);
            }
        }
        src = src + 8;
        dst = dst + 8;
    }
}
