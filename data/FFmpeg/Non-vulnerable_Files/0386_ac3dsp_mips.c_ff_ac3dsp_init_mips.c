void ff_ac3dsp_init_mips(AC3DSPContext *c, int bit_exact) {
#if HAVE_INLINE_ASM
#if HAVE_MIPSDSPR1
    c->bit_alloc_calc_bap = ac3_bit_alloc_calc_bap_mips;
    c->update_bap_counts  = ac3_update_bap_counts_mips;
#endif
#if HAVE_MIPSFPU
    c->float_to_fixed24 = float_to_fixed24_mips;
    c->downmix          = ac3_downmix_mips;
#endif
#endif
}
