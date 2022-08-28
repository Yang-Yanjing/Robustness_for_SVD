float ff_scalarproduct_float_neon(const float *v1, const float *v2, int len);
av_cold void ff_float_dsp_init_aarch64(AVFloatDSPContext *fdsp)
{
    int cpu_flags = av_get_cpu_flags();
    if (have_neon(cpu_flags)) {
        fdsp->butterflies_float   = ff_butterflies_float_neon;
        fdsp->scalarproduct_float = ff_scalarproduct_float_neon;
        fdsp->vector_dmul_scalar  = ff_vector_dmul_scalar_neon;
        fdsp->vector_fmul         = ff_vector_fmul_neon;
        fdsp->vector_fmac_scalar  = ff_vector_fmac_scalar_neon;
        fdsp->vector_fmul_add     = ff_vector_fmul_add_neon;
        fdsp->vector_fmul_reverse = ff_vector_fmul_reverse_neon;
        fdsp->vector_fmul_scalar  = ff_vector_fmul_scalar_neon;
        fdsp->vector_fmul_window  = ff_vector_fmul_window_neon;
    }
}
