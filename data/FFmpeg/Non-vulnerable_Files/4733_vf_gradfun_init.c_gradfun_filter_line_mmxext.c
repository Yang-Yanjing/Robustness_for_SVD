static void gradfun_filter_line_mmxext(uint8_t *dst, const uint8_t *src,
                                       const uint16_t *dc,
                                       int width, int thresh,
                                       const uint16_t *dithers)
{
    intptr_t x;
    if (width & 3) {
        x = width & ~3;
        ff_gradfun_filter_line_c(dst + x, src + x, dc + x / 2,
                                 width - x, thresh, dithers);
        width = x;
    }
    x = -width;
    ff_gradfun_filter_line_mmxext(x, dst + width, src + width, dc + width / 2,
                                  thresh, dithers);
}
