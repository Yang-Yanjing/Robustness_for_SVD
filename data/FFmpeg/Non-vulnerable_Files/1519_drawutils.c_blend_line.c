static void blend_line(uint8_t *dst, unsigned src, unsigned alpha,
                       int dx, int w, unsigned hsub, int left, int right)
{
    unsigned asrc = alpha * src;
    unsigned tau = 0x1010101 - alpha;
    int x;
    if (left) {
        unsigned suba = (left * alpha) >> hsub;
        *dst = (*dst * (0x1010101 - suba) + src * suba) >> 24;
        dst += dx;
    }
    for (x = 0; x < w; x++) {
        *dst = (*dst * tau + asrc) >> 24;
        dst += dx;
    }
    if (right) {
        unsigned suba = (right * alpha) >> hsub;
        *dst = (*dst * (0x1010101 - suba) + src * suba) >> 24;
    }
}
