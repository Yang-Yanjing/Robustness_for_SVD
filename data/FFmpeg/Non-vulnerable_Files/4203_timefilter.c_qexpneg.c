static double qexpneg(double x)
{
    return 1 - 1 / (1 + x * (1 + x / 2 * (1 + x / 3)));
}
