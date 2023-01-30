static inline int calc_lowcomp1(int a, int b0, int b1, int c)
{
    if ((b0 + 256) == b1) {
        a = c;
    } else if (b0 > b1) {
        a = FFMAX(a - 64, 0);
    }
    return a;
}
