TimeFilter *ff_timefilter_new(double time_base,
                              double period,
                              double bandwidth)
{
    TimeFilter *self       = av_mallocz(sizeof(TimeFilter));
    double o               = 2 * M_PI * bandwidth * period * time_base;
    if (!self)
        return NULL;
    self->clock_period     = time_base;
    self->feedback2_factor = qexpneg(M_SQRT2 * o);
    self->feedback3_factor = qexpneg(o * o) / period;
    return self;
}
