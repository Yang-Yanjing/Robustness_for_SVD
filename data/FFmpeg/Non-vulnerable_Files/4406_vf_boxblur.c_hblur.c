static void hblur(uint8_t *dst, int dst_linesize, const uint8_t *src, int src_linesize,
                  int w, int h, int radius, int power, uint8_t *temp[2], int pixsize)
{
    int y;
    if (radius == 0 && dst == src)
        return;
    for (y = 0; y < h; y++)
        blur_power(dst + y*dst_linesize, pixsize, src + y*src_linesize, pixsize,
                   w, radius, power, temp, pixsize);
}
