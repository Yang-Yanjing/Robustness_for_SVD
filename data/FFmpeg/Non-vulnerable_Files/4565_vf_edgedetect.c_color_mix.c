static void color_mix(int w, int h,
                            uint8_t *dst, int dst_linesize,
                      const uint8_t *src, int src_linesize)
{
    int i, j;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++)
            dst[i] = (dst[i] + src[i]) >> 1;
        dst += dst_linesize;
        src += src_linesize;
    }
}
