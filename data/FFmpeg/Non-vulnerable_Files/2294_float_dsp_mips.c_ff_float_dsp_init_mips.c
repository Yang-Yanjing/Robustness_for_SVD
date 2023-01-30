void ff_float_dsp_init_mips(AVFloatDSPContext *fdsp) {
#if HAVE_INLINE_ASM && HAVE_MIPSFPU
    fdsp->vector_fmul = vector_fmul_mips;
    fdsp->vector_fmul_scalar  = vector_fmul_scalar_mips;
    fdsp->vector_fmul_window = vector_fmul_window_mips;
    fdsp->butterflies_float = butterflies_float_mips;
    fdsp->vector_fmul_reverse = vector_fmul_reverse_mips;
#endif 
}
