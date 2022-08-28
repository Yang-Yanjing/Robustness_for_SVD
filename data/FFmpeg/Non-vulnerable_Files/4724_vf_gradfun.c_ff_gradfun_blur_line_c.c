void ff_gradfun_blur_line_c(uint16_t *dc, uint16_t *buf, const uint16_t *buf1, const uint8_t *src, int src_linesize, int width)
{
    int x, v, old;
    for (x = 0; x < width; x++) {
        v = buf1[x] + src[2 * x] + src[2 * x + 1] + src[2 * x + src_linesize] + src[2 * x + 1 + src_linesize];
        old = buf[x];
        buf[x] = v;
        dc[x] = v - old;
    }
}
