static void yuv2planeX_altivec(const int16_t *filter, int filterSize,
                               const int16_t **src, uint8_t *dest, int dstW,
                               const uint8_t *dither, int offset)
{
    int dst_u = -(uintptr_t)dest & 15;
    int i;
    yuv2planeX_u(filter, filterSize, src, dest, dst_u, dither, offset, 0);
    for (i = dst_u; i < dstW - 15; i += 16)
        yuv2planeX_16_altivec(filter, filterSize, src, dest + i, dither,
                              offset, i);
    yuv2planeX_u(filter, filterSize, src, dest, dstW, dither, offset, i);
}
