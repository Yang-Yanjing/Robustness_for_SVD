int ff_pullup_filter_var_mmx (const uint8_t *a, const uint8_t *b, ptrdiff_t s);
av_cold void ff_pullup_init_x86(PullupContext *s)
{
#if HAVE_YASM
    int cpu_flags = av_get_cpu_flags();
    if (EXTERNAL_MMX(cpu_flags)) {
        s->diff = ff_pullup_filter_diff_mmx;
        s->comb = ff_pullup_filter_comb_mmx;
        s->var  = ff_pullup_filter_var_mmx;
    }
#endif
}
