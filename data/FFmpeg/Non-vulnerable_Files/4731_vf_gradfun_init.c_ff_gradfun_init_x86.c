av_cold void ff_gradfun_init_x86(GradFunContext *gf)
{
#if HAVE_YASM
    int cpu_flags = av_get_cpu_flags();
    if (EXTERNAL_MMXEXT(cpu_flags))
        gf->filter_line = gradfun_filter_line_mmxext;
    if (EXTERNAL_SSSE3(cpu_flags))
        gf->filter_line = gradfun_filter_line_ssse3;
    if (EXTERNAL_SSE2(cpu_flags))
        gf->blur_line = gradfun_blur_line_sse2;
#endif 
}
