float ff_ssim_end_line_sse4(const int (*sum0)[4], const int (*sum1)[4], int w);
void ff_ssim_init_x86(SSIMDSPContext *dsp)
{
    int cpu_flags = av_get_cpu_flags();
    if (ARCH_X86_64 && EXTERNAL_SSSE3(cpu_flags))
        dsp->ssim_4x4_line = ff_ssim_4x4_line_ssse3;
    if (EXTERNAL_SSE4(cpu_flags))
        dsp->ssim_end_line = ff_ssim_end_line_sse4;
    if (EXTERNAL_XOP(cpu_flags))
        dsp->ssim_4x4_line = ff_ssim_4x4_line_xop;
}
