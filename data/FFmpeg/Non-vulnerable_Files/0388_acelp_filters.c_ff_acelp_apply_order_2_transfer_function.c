void ff_acelp_apply_order_2_transfer_function(float *out, const float *in,
                                              const float zero_coeffs[2],
                                              const float pole_coeffs[2],
                                              float gain, float mem[2], int n)
{
    int i;
    float tmp;
    for (i = 0; i < n; i++) {
        tmp = gain * in[i] - pole_coeffs[0] * mem[0] - pole_coeffs[1] * mem[1];
        out[i] =       tmp + zero_coeffs[0] * mem[0] + zero_coeffs[1] * mem[1];
        mem[1] = mem[0];
        mem[0] = tmp;
    }
}
