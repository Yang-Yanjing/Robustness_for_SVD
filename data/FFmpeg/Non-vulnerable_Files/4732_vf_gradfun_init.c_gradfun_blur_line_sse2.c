static void gradfun_blur_line_sse2(uint16_t *dc, uint16_t *buf, const uint16_t *buf1,
                                   const uint8_t *src, int src_linesize, int width)
{
    intptr_t x = -2 * width;
    if (((intptr_t) src | src_linesize) & 15)
        ff_gradfun_blur_line_movdqu_sse2(x, buf + width, buf1 + width,
                                         dc + width, src + width * 2,
                                         src + width * 2 + src_linesize);
    else
        ff_gradfun_blur_line_movdqa_sse2(x, buf + width, buf1 + width,
                                         dc + width, src + width * 2,
                                         src + width * 2 + src_linesize);
}
