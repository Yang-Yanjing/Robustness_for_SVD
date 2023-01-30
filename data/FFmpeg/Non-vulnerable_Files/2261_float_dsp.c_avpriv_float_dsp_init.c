av_cold void avpriv_float_dsp_init(AVFloatDSPContext *fdsp, int bit_exact)
{
    fdsp->vector_fmul = vector_fmul_c;
    fdsp->vector_fmac_scalar = vector_fmac_scalar_c;
    fdsp->vector_fmul_scalar = vector_fmul_scalar_c;
    fdsp->vector_dmul_scalar = vector_dmul_scalar_c;
    fdsp->vector_fmul_window = vector_fmul_window_c;
    fdsp->vector_fmul_add = vector_fmul_add_c;
    fdsp->vector_fmul_reverse = vector_fmul_reverse_c;
    fdsp->butterflies_float = butterflies_float_c;
    fdsp->scalarproduct_float = avpriv_scalarproduct_float_c;
    if (ARCH_AARCH64)
        ff_float_dsp_init_aarch64(fdsp);
    if (ARCH_ARM)
        ff_float_dsp_init_arm(fdsp);
    if (ARCH_PPC)
        ff_float_dsp_init_ppc(fdsp, bit_exact);
    if (ARCH_X86)
        ff_float_dsp_init_x86(fdsp);
    if (ARCH_MIPS)
        ff_float_dsp_init_mips(fdsp);
}
