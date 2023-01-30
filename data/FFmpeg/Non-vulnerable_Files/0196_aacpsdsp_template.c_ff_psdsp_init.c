av_cold void AAC_RENAME(ff_psdsp_init)(PSDSPContext *s)
{
    s->add_squares            = ps_add_squares_c;
    s->mul_pair_single        = ps_mul_pair_single_c;
    s->hybrid_analysis        = ps_hybrid_analysis_c;
    s->hybrid_analysis_ileave = ps_hybrid_analysis_ileave_c;
    s->hybrid_synthesis_deint = ps_hybrid_synthesis_deint_c;
    s->decorrelate            = ps_decorrelate_c;
    s->stereo_interpolate[0]  = ps_stereo_interpolate_c;
    s->stereo_interpolate[1]  = ps_stereo_interpolate_ipdopd_c;
#if !USE_FIXED
    if (ARCH_ARM)
        ff_psdsp_init_arm(s);
    if (ARCH_MIPS)
        ff_psdsp_init_mips(s);
    if (ARCH_X86)
        ff_psdsp_init_x86(s);
#endif 
}
