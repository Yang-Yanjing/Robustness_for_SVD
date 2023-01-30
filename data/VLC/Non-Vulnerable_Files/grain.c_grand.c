/* Gaussian random value with a mean of 0 and a variance of 1 */
static void grand(double *r1, double *r2, uint32_t *seed)
{
    double s;
    double u1, u2;
    do {
        u1 = 2 * drand(seed) - 1;
        u2 = 2 * drand(seed) - 1;
        s = u1 * u1 + u2 * u2;
    } while (s >= 1.0);
    s = sqrt(-2 * log(s) / s);
    *r1 = u1 * s;
    *r2 = u2 * s;
}
