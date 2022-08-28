static av_always_inline void fillPlane(uint8_t *plane, int stride, int width,
                                       int height, int y, uint8_t val)
{
    int i;
    uint8_t *ptr = plane + stride * y;
    for (i = 0; i < height; i++) {
        memset(ptr, val, width);
        ptr += stride;
    }
}
