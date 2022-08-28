static av_always_inline int sad_wxh(const uint8_t *src1, ptrdiff_t stride1,
                                    const uint8_t *src2, ptrdiff_t stride2,
                                    int w, int h)
{
    int x, y, sum = 0;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++)
            sum += abs(src1[x] - src2[x]);
        src1 += stride1;
        src2 += stride2;
    }
    return sum;
}
