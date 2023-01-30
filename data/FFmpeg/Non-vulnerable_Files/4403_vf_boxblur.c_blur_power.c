static inline void blur_power(uint8_t *dst, int dst_step, const uint8_t *src, int src_step,
                              int len, int radius, int power, uint8_t *temp[2], int pixsize)
{
    uint8_t *a = temp[0], *b = temp[1];
    if (radius && power) {
        blur(a, pixsize, src, src_step, len, radius, pixsize);
        for (; power > 2; power--) {
            uint8_t *c;
            blur(b, pixsize, a, pixsize, len, radius, pixsize);
            c = a; a = b; b = c;
        }
        if (power > 1) {
            blur(dst, dst_step, a, pixsize, len, radius, pixsize);
        } else {
            int i;
            if (pixsize == 1) {
                for (i = 0; i < len; i++)
                    dst[i*dst_step] = a[i];
            } else
                for (i = 0; i < len; i++)
                    *(uint16_t*)(dst + i*dst_step) = ((uint16_t*)a)[i];
        }
    } else {
        int i;
        if (pixsize == 1) {
            for (i = 0; i < len; i++)
                dst[i*dst_step] = src[i*src_step];
        } else
            for (i = 0; i < len; i++)
                *(uint16_t*)(dst + i*dst_step) = *(uint16_t*)(src + i*src_step);
    }
}
