static int get_available_samples(MixContext *s)
{
    int i;
    int available_samples = INT_MAX;
    av_assert0(s->nb_inputs > 1);
    for (i = 1; i < s->nb_inputs; i++) {
        int nb_samples;
        if (s->input_state[i] == INPUT_OFF)
            continue;
        nb_samples = av_audio_fifo_size(s->fifos[i]);
        available_samples = FFMIN(available_samples, nb_samples);
    }
    if (available_samples == INT_MAX)
        return 0;
    return available_samples;
}
