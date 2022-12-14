static inline void compose(float *dst, const float *src_l, const float *src_h,
                           int linesize, int w)
{
    int x, i;
    for (x = 0; x < w; x++) {
        double sum_l = src_l[x * linesize] * icoeff[0][0];
        double sum_h = src_h[x * linesize] * icoeff[1][0];
        for (i = 1; i <= 4; i++) {
            const int x0 = avpriv_mirror(x - i, w - 1) * linesize;
            const int x1 = avpriv_mirror(x + i, w - 1) * linesize;
            sum_l += icoeff[0][i] * (src_l[x0] + src_l[x1]);
            sum_h += icoeff[1][i] * (src_h[x0] + src_h[x1]);
        }
        dst[x * linesize] = (sum_l + sum_h) * 0.5;
    }
}
