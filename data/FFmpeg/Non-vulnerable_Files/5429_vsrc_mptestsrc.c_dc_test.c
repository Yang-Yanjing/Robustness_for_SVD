static void dc_test(uint8_t *dst, int dst_linesize, int w, int h, int off)
{
    const int step = FFMAX(256/(w*h/256), 1);
    int x, y, color = off;
    for (y = 0; y < h; y += 16) {
        for (x = 0; x < w; x += 16) {
            draw_dc(dst + x + y*dst_linesize, dst_linesize, color, 8, 8);
            color += step;
        }
    }
}
