                                   const uint8_t *src2, ptrdiff_t stride2);
void ff_pixelutils_sad_init_x86(av_pixelutils_sad_fn *sad, int aligned)
{
    int cpu_flags = av_get_cpu_flags();
    if (EXTERNAL_MMX(cpu_flags)) {
        sad[2] = ff_pixelutils_sad_8x8_mmx;
    }
    
    
    
    
    
    if (EXTERNAL_MMXEXT(cpu_flags)) {
        sad[2] = ff_pixelutils_sad_8x8_mmxext;
        sad[3] = ff_pixelutils_sad_16x16_mmxext;
    }
    if (EXTERNAL_SSE2(cpu_flags)) {
        switch (aligned) {
        case 0: sad[3] = ff_pixelutils_sad_16x16_sse2;   break; 
        case 1: sad[3] = ff_pixelutils_sad_u_16x16_sse2; break; 
        case 2: sad[3] = ff_pixelutils_sad_a_16x16_sse2; break; 
        }
    }
}
