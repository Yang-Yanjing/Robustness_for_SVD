static void subsampling_bounds(int sub, int *x, int *w, int *start, int *end)
{
    int mask = (1 << sub) - 1;
    *start = (-*x) & mask;
    *x += *start;
    *start = FFMIN(*start, *w);
    *w -= *start;
    *end = *w & mask;
    *w >>= sub;
}
