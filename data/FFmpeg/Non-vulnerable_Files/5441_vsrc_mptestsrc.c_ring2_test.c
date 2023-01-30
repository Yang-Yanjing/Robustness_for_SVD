static void ring2_test(uint8_t *dst, int dst_linesize, int off)
{
    int x, y;
    for (y = 0; y < 16*16; y++) {
        for (x = 0; x < 16*16; x++) {
            double d = sqrt((x-8*16)*(x-8*16) + (y-8*16)*(y-8*16));
            double r = d/20 - (int)(d/20);
            if (r < off/30.0) {
                dst[x + y*dst_linesize]     = 255;
                dst[x + y*dst_linesize+256] = 0;
            } else {
                dst[x + y*dst_linesize]     = x;
                dst[x + y*dst_linesize+256] = x;
            }
        }
    }
}
