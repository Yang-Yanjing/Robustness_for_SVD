static inline void blur16(uint16_t *dst, int dst_step, const uint16_t *src, int src_step,
                          int len, int radius)
{
    const int length = radius*2 + 1;
    const int inv = ((1<<16) + length/2)/length;
    int x, sum = src[radius*src_step];
    for (x = 0; x < radius; x++)
        sum += src[x*src_step]<<1;
    sum = sum*inv + (1<<15);
    for (x = 0; x <= radius; x++) {
        sum += (src[(radius+x)*src_step] - src[(radius-x)*src_step])*inv;
        dst[x*dst_step] = sum>>16;
    }
    for (; x < len-radius; x++) {
        sum += (src[(radius+x)*src_step] - src[(x-radius-1)*src_step])*inv;
        dst[x*dst_step] = sum >>16;
    }
    for (; x < len; x++) {
        sum += (src[(2*len-radius-x-1)*src_step] - src[(x-radius-1)*src_step])*inv;
        dst[x*dst_step] = sum>>16;
    }
}
