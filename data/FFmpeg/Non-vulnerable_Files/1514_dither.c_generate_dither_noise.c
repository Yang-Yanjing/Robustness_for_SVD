static int generate_dither_noise(DitherContext *c, DitherState *state,
                                 int min_samples)
{
    int i;
    int nb_samples  = FFALIGN(min_samples, 16) + 16;
    int buf_samples = nb_samples *
                      (c->method == AV_RESAMPLE_DITHER_RECTANGULAR ? 1 : 2);
    unsigned int *noise_buf_ui;
    av_freep(&state->noise_buf);
    state->noise_buf_size = state->noise_buf_ptr = 0;
    state->noise_buf = av_malloc(buf_samples * sizeof(*state->noise_buf));
    if (!state->noise_buf)
        return AVERROR(ENOMEM);
    state->noise_buf_size = FFALIGN(min_samples, 16);
    noise_buf_ui          = (unsigned int *)state->noise_buf;
    av_lfg_init(&state->lfg, state->seed);
    for (i = 0; i < buf_samples; i++)
        noise_buf_ui[i] = av_lfg_get(&state->lfg);
    c->ddsp.dither_int_to_float(state->noise_buf, noise_buf_ui, nb_samples);
    if (c->method == AV_RESAMPLE_DITHER_TRIANGULAR_HP)
        dither_highpass_filter(state->noise_buf, nb_samples);
    return 0;
}
