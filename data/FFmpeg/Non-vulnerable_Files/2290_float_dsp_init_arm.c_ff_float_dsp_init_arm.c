av_cold void ff_float_dsp_init_arm(AVFloatDSPContext *fdsp)
{
    int cpu_flags = av_get_cpu_flags();
    if (have_vfp(cpu_flags))
        ff_float_dsp_init_vfp(fdsp, cpu_flags);
    if (have_neon(cpu_flags))
        ff_float_dsp_init_neon(fdsp);
}
