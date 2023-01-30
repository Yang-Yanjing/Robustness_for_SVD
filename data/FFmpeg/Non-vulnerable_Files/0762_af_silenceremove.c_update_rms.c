static void update_rms(SilenceRemoveContext *s, double sample)
{
    s->rms_sum -= *s->window_current;
    *s->window_current = sample * sample;
    s->rms_sum += *s->window_current;
    s->window_current++;
    if (s->window_current >= s->window_end)
        s->window_current = s->window;
}
