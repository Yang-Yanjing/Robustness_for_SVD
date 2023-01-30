double ff_timefilter_update(TimeFilter *self, double system_time, double period)
{
    self->count++;
    if (self->count == 1) {
        self->cycle_time = system_time;
    } else {
        double loop_error;
        self->cycle_time += self->clock_period * period;
        loop_error = system_time - self->cycle_time;
        self->cycle_time   += FFMAX(self->feedback2_factor, 1.0 / self->count) * loop_error;
        self->clock_period += self->feedback3_factor * loop_error;
    }
    return self->cycle_time;
}
