static void draw_dc(uint8_t *dst, int dst_linesize, int color, int w, int h)
{
    int x, y;
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            dst[x + y*dst_linesize] = color;
}
