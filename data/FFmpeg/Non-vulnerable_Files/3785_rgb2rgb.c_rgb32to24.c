void rgb32to24(const uint8_t *src, uint8_t *dst, int src_size)
{
    int i, num_pixels = src_size >> 2;
    for (i = 0; i < num_pixels; i++) {
#if HAVE_BIGENDIAN
        
        dst[3 * i + 0] = src[4 * i + 1];
        dst[3 * i + 1] = src[4 * i + 2];
        dst[3 * i + 2] = src[4 * i + 3];
#else
        dst[3 * i + 0] = src[4 * i + 2];
        dst[3 * i + 1] = src[4 * i + 1];
        dst[3 * i + 2] = src[4 * i + 0];
#endif
    }
}
