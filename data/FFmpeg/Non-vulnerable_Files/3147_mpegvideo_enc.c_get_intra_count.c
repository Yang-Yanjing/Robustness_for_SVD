static int get_intra_count(MpegEncContext *s, uint8_t *src,
                           uint8_t *ref, int stride)
{
    int x, y, w, h;
    int acc = 0;
    w = s->width  & ~15;
    h = s->height & ~15;
    for (y = 0; y < h; y += 16) {
        for (x = 0; x < w; x += 16) {
            int offset = x + y * stride;
            int sad  = s->mecc.sad[0](NULL, src + offset, ref + offset,
                                      stride, 16);
            int mean = (s->mpvencdsp.pix_sum(src + offset, stride) + 128) >> 8;
            int sae  = get_sae(src + offset, mean, stride);
            acc += sae + 500 < sad;
        }
    }
    return acc;
}
