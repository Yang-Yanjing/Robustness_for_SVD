void ff_acelp_interpolate(int16_t* out, const int16_t* in,
                          const int16_t* filter_coeffs, int precision,
                          int frac_pos, int filter_length, int length)
{
    int n, i;
    av_assert1(frac_pos >= 0 && frac_pos < precision);
    for (n = 0; n < length; n++) {
        int idx = 0;
        int v = 0x4000;
        for (i = 0; i < filter_length;) {
            



            


            v += in[n + i] * filter_coeffs[idx + frac_pos];
            idx += precision;
            i++;
            v += in[n - i] * filter_coeffs[idx - frac_pos];
        }
        if (av_clip_int16(v >> 15) != (v >> 15))
            av_log(NULL, AV_LOG_WARNING, "overflow that would need cliping in ff_acelp_interpolate()\n");
        out[n] = v >> 15;
    }
}
};
void ff_acelp_interpolate(int16_t* out, const int16_t* in,
                          const int16_t* filter_coeffs, int precision,
                          int frac_pos, int filter_length, int length)
{
    int n, i;
    av_assert1(frac_pos >= 0 && frac_pos < precision);
    for (n = 0; n < length; n++) {
        int idx = 0;
        int v = 0x4000;
        for (i = 0; i < filter_length;) {
            



            


            v += in[n + i] * filter_coeffs[idx + frac_pos];
            idx += precision;
            i++;
            v += in[n - i] * filter_coeffs[idx - frac_pos];
        }
        if (av_clip_int16(v >> 15) != (v >> 15))
            av_log(NULL, AV_LOG_WARNING, "overflow that would need cliping in ff_acelp_interpolate()\n");
        out[n] = v >> 15;
    }
