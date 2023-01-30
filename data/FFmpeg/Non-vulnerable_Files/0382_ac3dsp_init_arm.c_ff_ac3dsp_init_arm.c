void ff_ac3_update_bap_counts_arm(uint16_t mant_cnt[16], uint8_t *bap, int len);
av_cold void ff_ac3dsp_init_arm(AC3DSPContext *c, int bit_exact)
{
    int cpu_flags = av_get_cpu_flags();
    c->update_bap_counts         = ff_ac3_update_bap_counts_arm;
    if (have_armv6(cpu_flags)) {
        c->bit_alloc_calc_bap    = ff_ac3_bit_alloc_calc_bap_armv6;
    }
    if (have_neon(cpu_flags)) {
        c->ac3_exponent_min      = ff_ac3_exponent_min_neon;
        c->ac3_max_msb_abs_int16 = ff_ac3_max_msb_abs_int16_neon;
        c->ac3_lshift_int16      = ff_ac3_lshift_int16_neon;
        c->ac3_rshift_int32      = ff_ac3_rshift_int32_neon;
        c->float_to_fixed24      = ff_float_to_fixed24_neon;
        c->extract_exponents     = ff_ac3_extract_exponents_neon;
        c->apply_window_int16    = ff_apply_window_int16_neon;
        c->sum_square_butterfly_int32 = ff_ac3_sum_square_butterfly_int32_neon;
        c->sum_square_butterfly_float = ff_ac3_sum_square_butterfly_float_neon;
    }
}
