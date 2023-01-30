 */
void fft_perform(const sound_sample *input, float *output, fft_state *state) {
    /* Convert data from sound format to be ready for FFT */
    fft_prepare(input, state->real, state->imag, state->bitReverse );
    /* Do the actual FFT */
    fft_calculate(state->real, state->imag, state->costable, state->sintable);
    /* Convert the FFT output into intensities */
    fft_output(state->real, state->imag, output);
}
