 */
fft_state *visual_fft_init(void)
{
    fft_state *p_state;
    unsigned int i;
    p_state = malloc( sizeof(*p_state) );
    if(! p_state )
        return NULL;
    for(i = 0; i < FFT_BUFFER_SIZE; i++)
    {
        p_state->bitReverse[i] = reverseBits(i);
    }
    for(i = 0; i < FFT_BUFFER_SIZE / 2; i++)
    {
        float j = 2 * PI * i / FFT_BUFFER_SIZE;
        p_state->costable[i] = cos(j);
        p_state->sintable[i] = sin(j);
    }
    return p_state;
}
