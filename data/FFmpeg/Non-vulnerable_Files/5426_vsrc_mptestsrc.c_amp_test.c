static void amp_test(uint8_t *dst, int dst_linesize, int off)
{
    int x, y, amp = off;
    for (y = 0; y < 16*16; y += 16) {
        for (x = 0; x < 16*16; x += 16) {
            draw_basis(dst + x + y*dst_linesize, dst_linesize, 4*amp, 1, 128*8);
            amp++;
        }
    }
}
