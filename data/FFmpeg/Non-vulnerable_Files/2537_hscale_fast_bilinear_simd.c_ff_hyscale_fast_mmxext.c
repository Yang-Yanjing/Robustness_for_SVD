void ff_hyscale_fast_mmxext(SwsContext *c, int16_t *dst,
                                 int dstWidth, const uint8_t *src,
                                 int srcW, int xInc)
{
    int32_t *filterPos = c->hLumFilterPos;
    int16_t *filter    = c->hLumFilter;
    void    *mmxextFilterCode = c->lumMmxextFilterCode;
    int i;
#if ARCH_X86_64
    uint64_t retsave;
#else
#if defined(PIC)
    uint64_t ebxsave;
#endif
#endif
    __asm__ volatile(
#if ARCH_X86_64
        "mov               -8(%%rsp), %%"REG_a" \n\t"
        "mov               %%"REG_a", %5        \n\t"  
#else
#if defined(PIC)
        "mov               %%"REG_b", %5        \n\t"  
#endif
#endif
        "pxor                  %%mm7, %%mm7     \n\t"
        "mov                      %0, %%"REG_c" \n\t"
        "mov                      %1, %%"REG_D" \n\t"
        "mov                      %2, %%"REG_d" \n\t"
        "mov                      %3, %%"REG_b" \n\t"
        "xor               %%"REG_a", %%"REG_a" \n\t" 
        PREFETCH"        (%%"REG_c")            \n\t"
        PREFETCH"      32(%%"REG_c")            \n\t"
        PREFETCH"      64(%%"REG_c")            \n\t"
#if ARCH_X86_64
#define CALL_MMXEXT_FILTER_CODE \
        "movl            (%%"REG_b"), %%esi     \n\t"\
        "call                    *%4            \n\t"\
        "movl (%%"REG_b", %%"REG_a"), %%esi     \n\t"\
        "add               %%"REG_S", %%"REG_c" \n\t"\
        "add               %%"REG_a", %%"REG_D" \n\t"\
        "xor               %%"REG_a", %%"REG_a" \n\t"\
#else
#define CALL_MMXEXT_FILTER_CODE \
        "movl (%%"REG_b"), %%esi        \n\t"\
        "call         *%4                       \n\t"\
        "addl (%%"REG_b", %%"REG_a"), %%"REG_c" \n\t"\
        "add               %%"REG_a", %%"REG_D" \n\t"\
        "xor               %%"REG_a", %%"REG_a" \n\t"\
#endif 
        CALL_MMXEXT_FILTER_CODE
        CALL_MMXEXT_FILTER_CODE
        CALL_MMXEXT_FILTER_CODE
        CALL_MMXEXT_FILTER_CODE
        CALL_MMXEXT_FILTER_CODE
        CALL_MMXEXT_FILTER_CODE
        CALL_MMXEXT_FILTER_CODE
        CALL_MMXEXT_FILTER_CODE
#if ARCH_X86_64
        "mov                      %5, %%"REG_a" \n\t"
        "mov               %%"REG_a", -8(%%rsp) \n\t"
#else
#if defined(PIC)
        "mov                      %5, %%"REG_b" \n\t"
#endif
#endif
        :: "m" (src), "m" (dst), "m" (filter), "m" (filterPos),
           "m" (mmxextFilterCode)
#if ARCH_X86_64
          ,"m"(retsave)
#else
#if defined(PIC)
          ,"m" (ebxsave)
#endif
#endif
        : "%"REG_a, "%"REG_c, "%"REG_d, "%"REG_S, "%"REG_D
#if ARCH_X86_64 || !defined(PIC)
         ,"%"REG_b
#endif
    );
    for (i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--)
        dst[i] = src[srcW-1]*128;
}
