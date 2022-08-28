av_cold void ff_idet_init_x86(IDETContext *idet, int for_16b)
{
#if HAVE_YASM
    const int cpu_flags = av_get_cpu_flags();
#if ARCH_X86_32
    if (EXTERNAL_MMX(cpu_flags)) {
        idet->filter_line = for_16b ? (ff_idet_filter_func)idet_filter_line_16bit_mmx : idet_filter_line_mmx;
    }
    if (EXTERNAL_MMXEXT(cpu_flags)) {
        idet->filter_line = for_16b ? (ff_idet_filter_func)idet_filter_line_16bit_mmx : idet_filter_line_mmxext;
    }
#endif 
    if (EXTERNAL_SSE2(cpu_flags)) {
        idet->filter_line = for_16b ? (ff_idet_filter_func)idet_filter_line_16bit_sse2 : idet_filter_line_sse2;
    }
#endif 
}
