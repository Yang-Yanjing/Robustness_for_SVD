static inline double get_coeff(double d)
{
    double coeff, A = -0.60;
    d = fabs(d);
    if (d < 1.0)
        coeff = (1.0 - (A + 3.0) * d * d + (A + 2.0) * d * d * d);
    else if (d < 2.0)
        coeff = (-4.0 * A + 8.0 * A * d - 5.0 * A * d * d + A * d * d * d);
    else
        coeff = 0.0;
    return coeff;
}
