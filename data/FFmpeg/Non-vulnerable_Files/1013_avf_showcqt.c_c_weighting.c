static double c_weighting(void *p, double f)
{
    double ret = 12200.0*12200.0 * (f*f);
    ret /= (f*f + 20.6*20.6) * (f*f + 12200.0*12200.0);
    return ret;
}
