AVFixedDSPContext * avpriv_alloc_fixed_dsp(int bit_exact)
{
    AVFixedDSPContext * fdsp = av_malloc(sizeof(AVFixedDSPContext));
    if (!fdsp)
        return NULL;
    fdsp->vector_fmul_window_scaled = vector_fmul_window_scaled_c;
    fdsp->vector_fmul_window = vector_fmul_window_c;
    fdsp->vector_fmul = vector_fmul_c;
    fdsp->vector_fmul_add = vector_fmul_add_c;
    fdsp->vector_fmul_reverse = vector_fmul_reverse_c;
    fdsp->butterflies_fixed = butterflies_fixed_c;
    fdsp->scalarproduct_fixed = scalarproduct_fixed_c;
    return fdsp;
}
