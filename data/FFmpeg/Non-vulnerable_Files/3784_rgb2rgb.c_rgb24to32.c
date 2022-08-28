void rgb24to32(const uint8_t *src, uint8_t *dst, int src_size)
{
    int i;
    for (i = 0; 3 * i < src_size; i++) {
#if HAVE_BIGENDIAN
        
        dst[4 * i + 0] = 255;
        dst[4 * i + 1] = src[3 * i + 0];
        dst[4 * i + 2] = src[3 * i + 1];
        dst[4 * i + 3] = src[3 * i + 2];
#else
        dst[4 * i + 0] = src[3 * i + 2];
        dst[4 * i + 1] = src[3 * i + 1];
        dst[4 * i + 2] = src[3 * i + 0];
        dst[4 * i + 3] = 255;
#endif
    }
}
