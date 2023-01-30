static inline uint8_t ana_convert(const int *coeff, const uint8_t *left, const uint8_t *right)
{
    int sum;
    sum  = coeff[0] * left[0] + coeff[3] * right[0]; 
    sum += coeff[1] * left[1] + coeff[4] * right[1]; 
    sum += coeff[2] * left[2] + coeff[5] * right[2]; 
    return av_clip_uint8(sum >> 16);
}
