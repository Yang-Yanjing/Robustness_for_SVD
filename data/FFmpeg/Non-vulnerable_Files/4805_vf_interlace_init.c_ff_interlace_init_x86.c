                          const uint8_t *srcp_below);
av_cold void ff_interlace_init_x86(InterlaceContext *s)
{
    int cpu_flags = av_get_cpu_flags();
    if (EXTERNAL_SSE2(cpu_flags))
        s->lowpass_line = ff_lowpass_line_sse2;
    if (EXTERNAL_AVX(cpu_flags))
        s->lowpass_line = ff_lowpass_line_avx;
}
