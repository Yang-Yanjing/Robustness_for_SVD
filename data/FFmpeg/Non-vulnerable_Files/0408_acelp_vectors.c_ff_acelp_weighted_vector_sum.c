void ff_acelp_weighted_vector_sum(
        int16_t* out,
        const int16_t *in_a,
        const int16_t *in_b,
        int16_t weight_coeff_a,
        int16_t weight_coeff_b,
        int16_t rounder,
        int shift,
        int length)
{
    int i;

    
    for(i=0; i<length; i++)
        out[i] = av_clip_int16((
                 in_a[i] * weight_coeff_a +
                 in_b[i] * weight_coeff_b +
                 rounder) >> shift);
}