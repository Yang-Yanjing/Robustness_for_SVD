static inline double fade(double prev, double next, int pos,
                          double *fade_factors[2])
{
    return fade_factors[0][pos] * prev + fade_factors[1][pos] * next;
}
