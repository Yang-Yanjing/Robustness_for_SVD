 */
static void fft_prepare( const sound_sample *input, float * re, float * im,
                         const unsigned int *bitReverse ) {
    unsigned int i;
    float *p_real = re;
    float *p_imag = im;
    /* Get input, in reverse bit order */
    for(i = 0; i < FFT_BUFFER_SIZE; i++)
    {
        *p_real++ = input[bitReverse[i]];
        *p_imag++ = 0;
    }
}
