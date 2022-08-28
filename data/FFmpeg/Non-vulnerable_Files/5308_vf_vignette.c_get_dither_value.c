static inline double get_dither_value(VignetteContext *s)
{
    double dv = 0;
    if (s->do_dither) {
        dv = s->dither / (double)(1LL<<32);
        s->dither = s->dither * 1664525 + 1013904223;
    }
    return dv;
}
