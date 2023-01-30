av_cold void ff_ac3dsp_init_x86(AC3DSPContext *c, int bit_exact)
{
    int cpu_flags = av_get_cpu_flags();
    if (EXTERNAL_MMX(cpu_flags)) {
        c->ac3_exponent_min = ff_ac3_exponent_min_mmx;
        c->ac3_max_msb_abs_int16 = ff_ac3_max_msb_abs_int16_mmx;
        c->ac3_lshift_int16 = ff_ac3_lshift_int16_mmx;
        c->ac3_rshift_int32 = ff_ac3_rshift_int32_mmx;
    }
    if (EXTERNAL_AMD3DNOW(cpu_flags)) {
        if (!bit_exact) {
            c->float_to_fixed24 = ff_float_to_fixed24_3dnow;
        }
    }
    if (EXTERNAL_MMXEXT(cpu_flags)) {
        c->ac3_exponent_min = ff_ac3_exponent_min_mmxext;
        c->ac3_max_msb_abs_int16 = ff_ac3_max_msb_abs_int16_mmxext;
        if (bit_exact) {
            c->apply_window_int16 = ff_apply_window_int16_mmxext;
        } else {
            c->apply_window_int16 = ff_apply_window_int16_round_mmxext;
        }
    }
    if (EXTERNAL_SSE(cpu_flags)) {
        c->float_to_fixed24 = ff_float_to_fixed24_sse;
    }
    if (EXTERNAL_SSE2(cpu_flags)) {
        c->ac3_exponent_min = ff_ac3_exponent_min_sse2;
        c->ac3_max_msb_abs_int16 = ff_ac3_max_msb_abs_int16_sse2;
        c->float_to_fixed24 = ff_float_to_fixed24_sse2;
        c->compute_mantissa_size = ff_ac3_compute_mantissa_size_sse2;
        c->extract_exponents = ff_ac3_extract_exponents_sse2;
        if (!(cpu_flags & AV_CPU_FLAG_SSE2SLOW)) {
            c->ac3_lshift_int16 = ff_ac3_lshift_int16_sse2;
            c->ac3_rshift_int32 = ff_ac3_rshift_int32_sse2;
        }
        if (bit_exact) {
            c->apply_window_int16 = ff_apply_window_int16_sse2;
        } else if (!(cpu_flags & AV_CPU_FLAG_SSE2SLOW)) {
            c->apply_window_int16 = ff_apply_window_int16_round_sse2;
        }
    }
    if (EXTERNAL_SSSE3(cpu_flags)) {
        c->ac3_max_msb_abs_int16 = ff_ac3_max_msb_abs_int16_ssse3;
        if (cpu_flags & AV_CPU_FLAG_ATOM) {
            c->apply_window_int16 = ff_apply_window_int16_ssse3_atom;
        } else {
            c->extract_exponents = ff_ac3_extract_exponents_ssse3;
            c->apply_window_int16 = ff_apply_window_int16_ssse3;
        }
    }
#if HAVE_SSE_INLINE && HAVE_7REGS
    if (INLINE_SSE(cpu_flags)) {
        c->downmix = ac3_downmix_sse;
    }
#endif
}
