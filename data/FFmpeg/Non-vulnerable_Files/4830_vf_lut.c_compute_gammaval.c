static double compute_gammaval(void *opaque, double gamma)
{
    LutContext *s = opaque;
    double val    = s->var_values[VAR_CLIPVAL];
    double minval = s->var_values[VAR_MINVAL];
    double maxval = s->var_values[VAR_MAXVAL];
    return pow((val-minval)/(maxval-minval), gamma) * (maxval-minval)+minval;
}
