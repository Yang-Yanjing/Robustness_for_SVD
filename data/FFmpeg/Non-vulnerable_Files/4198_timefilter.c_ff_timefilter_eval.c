double ff_timefilter_eval(TimeFilter *self, double delta)
{
    return self->cycle_time + self->clock_period * delta;
}
