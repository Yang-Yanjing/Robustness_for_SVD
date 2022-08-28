static void clear_rms(SilenceRemoveContext *s)
{
    memset(s->window, 0, s->window_size * sizeof(*s->window));
    s->window_current = s->window;
    s->window_end = s->window + s->window_size;
    s->rms_sum = 0;
}
