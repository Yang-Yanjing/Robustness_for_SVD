static void interleave(uint8_t *dst, uint8_t *src, int w, int h,
                       int dst_linesize, int src_linesize,
                       enum FilterMode mode, int swap)
{
    const int a = swap;
    const int b = 1 - a;
    const int m = h >> 1;
    int y;
    switch (mode) {
    case MODE_DEINTERLEAVE:
        for (y = 0; y < m; y++) {
            memcpy(dst + dst_linesize *  y     , src + src_linesize * (y * 2 + a), w);
            memcpy(dst + dst_linesize * (y + m), src + src_linesize * (y * 2 + b), w);
        }
        break;
    case MODE_NONE:
        for (y = 0; y < m; y++) {
            memcpy(dst + dst_linesize *  y * 2     , src + src_linesize * (y * 2 + a), w);
            memcpy(dst + dst_linesize * (y * 2 + 1), src + src_linesize * (y * 2 + b), w);
        }
        break;
    case MODE_INTERLEAVE:
        for (y = 0; y < m; y++) {
            memcpy(dst + dst_linesize * (y * 2 + a), src + src_linesize *  y     , w);
            memcpy(dst + dst_linesize * (y * 2 + b), src + src_linesize * (y + m), w);
        }
        break;
    }
}
