static double compute_gammaval709(void *opaque, double gamma)
{
    LutContext *s = opaque;
    double val    = s->var_values[VAR_CLIPVAL];
    double minval = s->var_values[VAR_MINVAL];
    double maxval = s->var_values[VAR_MAXVAL];
    double level = (val - minval) / (maxval - minval);
    level = level < 0.018 ? 4.5 * level
                          : 1.099 * pow(level, 1.0 / gamma) - 0.099;
    return level * (maxval - minval) + minval;
}
