static void quantize_triangular_ns(DitherContext *c, DitherState *state,
                                   int16_t *dst, const float *src,
                                   int nb_samples)
{
    int i, j;
    float *dither = &state->noise_buf[state->noise_buf_ptr];
    if (state->mute > c->mute_reset_threshold)
        memset(state->dither_a, 0, sizeof(state->dither_a));
    for (i = 0; i < nb_samples; i++) {
        float err = 0;
        float sample = src[i] * S16_SCALE;
        for (j = 0; j < 4; j++) {
            err += c->ns_coef_b[j] * state->dither_b[j] -
                   c->ns_coef_a[j] * state->dither_a[j];
        }
        for (j = 3; j > 0; j--) {
            state->dither_a[j] = state->dither_a[j - 1];
            state->dither_b[j] = state->dither_b[j - 1];
        }
        state->dither_a[0] = err;
        sample -= err;
        if (state->mute > c->mute_dither_threshold) {
            dst[i]             = av_clip_int16(lrintf(sample));
            state->dither_b[0] = 0;
        } else {
            dst[i]             = av_clip_int16(lrintf(sample + dither[i]));
            state->dither_b[0] = av_clipf(dst[i] - sample, -1.5f, 1.5f);
        }
        state->mute++;
        if (src[i])
            state->mute = 0;
    }
}
