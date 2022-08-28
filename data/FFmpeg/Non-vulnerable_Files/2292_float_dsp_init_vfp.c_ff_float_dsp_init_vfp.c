void ff_butterflies_float_vfp(float *av_restrict v1, float *av_restrict v2, int len);
av_cold void ff_float_dsp_init_vfp(AVFloatDSPContext *fdsp, int cpu_flags)
{
    if (!have_vfpv3(cpu_flags)) {
        fdsp->vector_fmul = ff_vector_fmul_vfp;
        fdsp->vector_fmul_window = ff_vector_fmul_window_vfp;
    }
    fdsp->vector_fmul_reverse = ff_vector_fmul_reverse_vfp;
    if (!have_vfpv3(cpu_flags))
        fdsp->butterflies_float = ff_butterflies_float_vfp;
}
