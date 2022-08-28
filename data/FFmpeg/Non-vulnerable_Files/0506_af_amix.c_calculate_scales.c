static void calculate_scales(MixContext *s, int nb_samples)
{
    int i;
    if (s->scale_norm > s->active_inputs) {
        s->scale_norm -= nb_samples / (s->dropout_transition * s->sample_rate);
        s->scale_norm = FFMAX(s->scale_norm, s->active_inputs);
    }
    for (i = 0; i < s->nb_inputs; i++) {
        if (s->input_state[i] == INPUT_ON)
            s->input_scale[i] = 1.0f / s->scale_norm;
        else
            s->input_scale[i] = 0.0f;
    }
}
