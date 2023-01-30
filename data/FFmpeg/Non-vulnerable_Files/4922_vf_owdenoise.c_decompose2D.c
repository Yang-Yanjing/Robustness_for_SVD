static inline void decompose2D(float *dst_l, float *dst_h, const float *src,
                               int xlinesize, int ylinesize,
                               int step, int w, int h)
{
    int y, x;
    for (y = 0; y < h; y++)
        for (x = 0; x < step; x++)
            decompose(dst_l + ylinesize*y + xlinesize*x,
                      dst_h + ylinesize*y + xlinesize*x,
                      src   + ylinesize*y + xlinesize*x,
                      step * xlinesize, (w - x + step - 1) / step);
}
