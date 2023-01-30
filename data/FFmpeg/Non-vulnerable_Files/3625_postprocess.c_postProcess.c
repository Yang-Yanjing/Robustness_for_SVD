                      const QP_STORE_T QPs[], int QPStride, int isColor, PPContext *c2);
static inline void postProcess(const uint8_t src[], int srcStride, uint8_t dst[], int dstStride, int width, int height,
        const QP_STORE_T QPs[], int QPStride, int isColor, pp_mode *vm, pp_context *vc)
{
    pp_fn pp = postProcess_C;
    PPContext *c= (PPContext *)vc;
    PPMode *ppMode= (PPMode *)vm;
    c->ppMode= *ppMode; 
    if (!(ppMode->lumMode & BITEXACT)) {
#if CONFIG_RUNTIME_CPUDETECT
#if ARCH_X86 && HAVE_INLINE_ASM
        
        if      (c->cpuCaps & AV_CPU_FLAG_SSE2)     pp = postProcess_SSE2;
        else if (c->cpuCaps & AV_CPU_FLAG_MMXEXT)   pp = postProcess_MMX2;
        else if (c->cpuCaps & AV_CPU_FLAG_3DNOW)    pp = postProcess_3DNow;
        else if (c->cpuCaps & AV_CPU_FLAG_MMX)      pp = postProcess_MMX;
#elif HAVE_ALTIVEC
        if      (c->cpuCaps & AV_CPU_FLAG_ALTIVEC)  pp = postProcess_altivec;
#endif
#else 
#if     HAVE_SSE2_INLINE
        pp = postProcess_SSE2;
#elif   HAVE_MMXEXT_INLINE
        pp = postProcess_MMX2;
#elif HAVE_AMD3DNOW_INLINE
        pp = postProcess_3DNow;
#elif HAVE_MMX_INLINE
        pp = postProcess_MMX;
#elif HAVE_ALTIVEC
        pp = postProcess_altivec;
#endif
#endif 
    }
    pp(src, srcStride, dst, dstStride, width, height, QPs, QPStride, isColor, c);
}
