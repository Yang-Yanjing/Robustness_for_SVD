static inline void compose2D(float *dst, const float *src_l, const float *src_h,
                             int xlinesize, int ylinesize,
                             int step, int w, int h)
{
    int y, x;
    for (y = 0; y < h; y++)
        for (x = 0; x < step; x++)
            compose(dst   + ylinesize*y + xlinesize*x,
                    src_l + ylinesize*y + xlinesize*x,
                    src_h + ylinesize*y + xlinesize*x,
                    step * xlinesize, (w - x + step - 1) / step);
}
