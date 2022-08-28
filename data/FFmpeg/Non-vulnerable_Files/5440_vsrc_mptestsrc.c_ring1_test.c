static void ring1_test(uint8_t *dst, int dst_linesize, int off)
{
    int x, y, color = 0;
    for (y = off; y < 16*16; y += 16) {
        for (x = off; x < 16*16; x += 16) {
            draw_dc(dst + x + y*dst_linesize, dst_linesize, ((x+y)&16) ? color : -color, 16, 16);
            color++;
        }
    }
}
