static void noise(uint8_t *dst, const uint8_t *src,
                  int dst_linesize, int src_linesize,
                  int width, int start, int end, NoiseContext *n, int comp)
{
    FilterParams *p = &n->param[comp];
    int8_t *noise = p->noise;
    const int flags = p->flags;
    int y;
    if (!noise) {
        if (dst != src)
            av_image_copy_plane(dst, dst_linesize, src, src_linesize, width, end - start);
        return;
    }
    for (y = start; y < end; y++) {
        const int ix = y & (MAX_RES - 1);
        int x;
        for (x=0; x < width; x+= MAX_RES) {
            int w = FFMIN(width - x, MAX_RES);
            int shift = p->rand_shift[ix];
            if (flags & NOISE_AVERAGED) {
                n->line_noise_avg(dst + x, src + x, w, (const int8_t**)p->prev_shift[ix]);
                p->prev_shift[ix][shift & 3] = noise + shift;
            } else {
                n->line_noise(dst + x, src + x, noise, w, shift);
            }
        }
        dst += dst_linesize;
        src += src_linesize;
    }
}
