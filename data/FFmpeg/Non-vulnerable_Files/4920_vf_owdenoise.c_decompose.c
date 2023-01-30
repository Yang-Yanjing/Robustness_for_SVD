static inline void decompose(float *dst_l, float *dst_h, const float *src,
                             int linesize, int w)
{
    int x, i;
    for (x = 0; x < w; x++) {
        double sum_l = src[x * linesize] * coeff[0][0];
        double sum_h = src[x * linesize] * coeff[1][0];
        for (i = 1; i <= 4; i++) {
            const double s = src[avpriv_mirror(x - i, w - 1) * linesize]
                           + src[avpriv_mirror(x + i, w - 1) * linesize];
            sum_l += coeff[0][i] * s;
            sum_h += coeff[1][i] * s;
        }
        dst_l[x * linesize] = sum_l;
        dst_h[x * linesize] = sum_h;
    }
}
