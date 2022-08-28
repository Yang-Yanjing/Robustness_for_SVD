void ff_pp7_dctB_mmx(int16_t *dst, int16_t *src);
av_cold void ff_pp7_init_x86(PP7Context *p)
{
    int cpu_flags = av_get_cpu_flags();
    if (EXTERNAL_MMX(cpu_flags))
        p->dctB = ff_pp7_dctB_mmx;
}
