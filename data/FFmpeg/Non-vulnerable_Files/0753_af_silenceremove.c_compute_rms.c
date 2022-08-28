static double compute_rms(SilenceRemoveContext *s, double sample)
{
    double new_sum;
    new_sum  = s->rms_sum;
    new_sum -= *s->window_current;
    new_sum += sample * sample;
    return sqrt(new_sum / s->window_size);
}
