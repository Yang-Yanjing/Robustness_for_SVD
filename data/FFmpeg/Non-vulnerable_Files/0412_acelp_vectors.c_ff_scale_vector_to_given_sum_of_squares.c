void ff_scale_vector_to_given_sum_of_squares(float *out, const float *in,
                                             float sum_of_squares, const int n)
{
    int i;
    float scalefactor = avpriv_scalarproduct_float_c(in, in, n);
    if (scalefactor)
        scalefactor = sqrt(sum_of_squares / scalefactor);
    for (i = 0; i < n; i++)
        out[i] = in[i] * scalefactor;
}
