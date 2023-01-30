static double a_weighting(void *p, double f)
{
    double ret = 12200.0*12200.0 * (f*f*f*f);
    ret /= (f*f + 20.6*20.6) * (f*f + 12200.0*12200.0) *
           sqrt((f*f + 107.7*107.7) * (f*f + 737.9*737.9));
    return ret;
}
