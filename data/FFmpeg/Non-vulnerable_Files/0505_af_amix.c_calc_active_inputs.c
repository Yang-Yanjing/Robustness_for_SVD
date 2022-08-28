static int calc_active_inputs(MixContext *s)
{
    int i;
    int active_inputs = 0;
    for (i = 0; i < s->nb_inputs; i++)
        active_inputs += !!(s->input_state[i] != INPUT_OFF);
    s->active_inputs = active_inputs;
    if (!active_inputs ||
        (s->duration_mode == DURATION_FIRST && s->input_state[0] == INPUT_OFF) ||
        (s->duration_mode == DURATION_SHORTEST && active_inputs != s->nb_inputs))
        return AVERROR_EOF;
    return 0;
}
