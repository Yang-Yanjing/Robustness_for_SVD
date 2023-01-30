static int convert_samples(DitherContext *c, int16_t **dst, float * const *src,
                           int channels, int nb_samples)
{
    int ch, ret;
    int aligned_samples = FFALIGN(nb_samples, 16);
    for (ch = 0; ch < channels; ch++) {
        DitherState *state = &c->state[ch];
        if (state->noise_buf_size < aligned_samples) {
            ret = generate_dither_noise(c, state, nb_samples);
            if (ret < 0)
                return ret;
        } else if (state->noise_buf_size - state->noise_buf_ptr < aligned_samples) {
            state->noise_buf_ptr = 0;
        }
        if (c->method == AV_RESAMPLE_DITHER_TRIANGULAR_NS) {
            quantize_triangular_ns(c, state, dst[ch], src[ch], nb_samples);
        } else {
            c->quantize(dst[ch], src[ch],
                        &state->noise_buf[state->noise_buf_ptr],
                        FFALIGN(nb_samples, c->samples_align));
        }
        state->noise_buf_ptr += aligned_samples;
    }
    return 0;
}
