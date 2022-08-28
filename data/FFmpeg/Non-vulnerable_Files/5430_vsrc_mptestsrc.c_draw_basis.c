static void draw_basis(uint8_t *dst, int dst_linesize, int amp, int freq, int dc)
{
    int src[64];
    memset(src, 0, 64*sizeof(int));
    src[0] = dc;
    if (amp)
        src[freq] = amp;
    idct(dst, dst_linesize, src);
}
