 */
static void fft_output(const float * re, const float * im, float *output)
{
    float *p_output = output;
    const float *p_real   = re;
    const float *p_imag   = im;
    float *p_end    = output + FFT_BUFFER_SIZE / 2;
    while(p_output <= p_end)
    {
        *p_output = (*p_real * *p_real) + (*p_imag * *p_imag);
        p_output++; p_real++; p_imag++;
    }
    /* Do divisions to keep the constant and highest frequency terms in scale
     * with the other terms. */
    *output /= 4;
    *p_end /= 4;
}
