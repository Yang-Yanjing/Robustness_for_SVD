av_cold void ff_ac3dsp_init(AC3DSPContext *c, int bit_exact)
{
    c->ac3_exponent_min = ac3_exponent_min_c;
    c->ac3_max_msb_abs_int16 = ac3_max_msb_abs_int16_c;
    c->ac3_lshift_int16 = ac3_lshift_int16_c;
    c->ac3_rshift_int32 = ac3_rshift_int32_c;
    c->float_to_fixed24 = float_to_fixed24_c;
    c->bit_alloc_calc_bap = ac3_bit_alloc_calc_bap_c;
    c->update_bap_counts = ac3_update_bap_counts_c;
    c->compute_mantissa_size = ac3_compute_mantissa_size_c;
    c->extract_exponents = ac3_extract_exponents_c;
    c->sum_square_butterfly_int32 = ac3_sum_square_butterfly_int32_c;
    c->sum_square_butterfly_float = ac3_sum_square_butterfly_float_c;
    c->downmix = ac3_downmix_c;
    c->downmix_fixed = ac3_downmix_c_fixed;
    c->apply_window_int16 = apply_window_int16_c;
    if (ARCH_ARM)
        ff_ac3dsp_init_arm(c, bit_exact);
    if (ARCH_X86)
        ff_ac3dsp_init_x86(c, bit_exact);
    if (ARCH_MIPS)
        ff_ac3dsp_init_mips(c, bit_exact);
}
