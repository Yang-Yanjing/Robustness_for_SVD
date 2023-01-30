av_always_inline
static void denoise_temporal(uint8_t *src, uint8_t *dst,
                             uint16_t *frame_ant,
                             int w, int h, int sstride, int dstride,
                             int16_t *temporal, int depth)
{
    long x, y;
    uint32_t tmp;
    temporal += 256 << LUT_BITS;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            frame_ant[x] = tmp = lowpass(frame_ant[x], LOAD(x), temporal, depth);
            STORE(x, tmp);
        }
        src += sstride;
        dst += dstride;
        frame_ant += w;
    }
}
