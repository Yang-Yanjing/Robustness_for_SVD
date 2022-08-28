static inline void RENAME(blockCopy)(uint8_t dst[], int dstStride, const uint8_t src[], int srcStride,
                                     int levelFix, int64_t *packedOffsetAndScale)
{
#if !TEMPLATE_PP_MMX || !HAVE_6REGS
    int i;
#endif
    if(levelFix){
#if TEMPLATE_PP_MMX && HAVE_6REGS
    __asm__ volatile(
        "movq (%%"REG_a"), %%mm2        \n\t" 
        "movq 8(%%"REG_a"), %%mm3       \n\t" 
        "lea (%2,%4), %%"REG_a"         \n\t"
        "lea (%3,%5), %%"REG_d"         \n\t"
        "pxor %%mm4, %%mm4              \n\t"
#if TEMPLATE_PP_MMXEXT
#define REAL_SCALED_CPY(src1, src2, dst1, dst2)                                                \
        "movq " #src1 ", %%mm0          \n\t"\
        "movq " #src1 ", %%mm5          \n\t"\
        "movq " #src2 ", %%mm1          \n\t"\
        "movq " #src2 ", %%mm6          \n\t"\
        "punpcklbw %%mm0, %%mm0         \n\t"\
        "punpckhbw %%mm5, %%mm5         \n\t"\
        "punpcklbw %%mm1, %%mm1         \n\t"\
        "punpckhbw %%mm6, %%mm6         \n\t"\
        "pmulhuw %%mm3, %%mm0           \n\t"\
        "pmulhuw %%mm3, %%mm5           \n\t"\
        "pmulhuw %%mm3, %%mm1           \n\t"\
        "pmulhuw %%mm3, %%mm6           \n\t"\
        "psubw %%mm2, %%mm0             \n\t"\
        "psubw %%mm2, %%mm5             \n\t"\
        "psubw %%mm2, %%mm1             \n\t"\
        "psubw %%mm2, %%mm6             \n\t"\
        "packuswb %%mm5, %%mm0          \n\t"\
        "packuswb %%mm6, %%mm1          \n\t"\
        "movq %%mm0, " #dst1 "          \n\t"\
        "movq %%mm1, " #dst2 "          \n\t"\
#else 
#define REAL_SCALED_CPY(src1, src2, dst1, dst2)                                        \
        "movq " #src1 ", %%mm0          \n\t"\
        "movq " #src1 ", %%mm5          \n\t"\
        "punpcklbw %%mm4, %%mm0         \n\t"\
        "punpckhbw %%mm4, %%mm5         \n\t"\
        "psubw %%mm2, %%mm0             \n\t"\
        "psubw %%mm2, %%mm5             \n\t"\
        "movq " #src2 ", %%mm1          \n\t"\
        "psllw $6, %%mm0                \n\t"\
        "psllw $6, %%mm5                \n\t"\
        "pmulhw %%mm3, %%mm0            \n\t"\
        "movq " #src2 ", %%mm6          \n\t"\
        "pmulhw %%mm3, %%mm5            \n\t"\
        "punpcklbw %%mm4, %%mm1         \n\t"\
        "punpckhbw %%mm4, %%mm6         \n\t"\
        "psubw %%mm2, %%mm1             \n\t"\
        "psubw %%mm2, %%mm6             \n\t"\
        "psllw $6, %%mm1                \n\t"\
        "psllw $6, %%mm6                \n\t"\
        "pmulhw %%mm3, %%mm1            \n\t"\
        "pmulhw %%mm3, %%mm6            \n\t"\
        "packuswb %%mm5, %%mm0          \n\t"\
        "packuswb %%mm6, %%mm1          \n\t"\
        "movq %%mm0, " #dst1 "          \n\t"\
        "movq %%mm1, " #dst2 "          \n\t"\
#endif 
#define SCALED_CPY(src1, src2, dst1, dst2)\
   REAL_SCALED_CPY(src1, src2, dst1, dst2)
SCALED_CPY((%2)       , (%2, %4)      , (%3)       , (%3, %5))
SCALED_CPY((%2, %4, 2), (%%REGa, %4, 2), (%3, %5, 2), (%%REGd, %5, 2))
SCALED_CPY((%2, %4, 4), (%%REGa, %4, 4), (%3, %5, 4), (%%REGd, %5, 4))
        "lea (%%"REG_a",%4,4), %%"REG_a"        \n\t"
        "lea (%%"REG_d",%5,4), %%"REG_d"        \n\t"
SCALED_CPY((%%REGa, %4), (%%REGa, %4, 2), (%%REGd, %5), (%%REGd, %5, 2))
        : "=&a" (packedOffsetAndScale)
        : "0" (packedOffsetAndScale),
        "r"(src),
        "r"(dst),
        "r" ((x86_reg)srcStride),
        "r" ((x86_reg)dstStride)
        : "%"REG_d
    );
#else 
    for(i=0; i<8; i++)
        memcpy( &(dst[dstStride*i]),
                &(src[srcStride*i]), BLOCK_SIZE);
#endif 
    }else{
#if TEMPLATE_PP_MMX && HAVE_6REGS
    __asm__ volatile(
        "lea (%0,%2), %%"REG_a"                 \n\t"
        "lea (%1,%3), %%"REG_d"                 \n\t"
#define REAL_SIMPLE_CPY(src1, src2, dst1, dst2)                              \
        "movq " #src1 ", %%mm0          \n\t"\
        "movq " #src2 ", %%mm1          \n\t"\
        "movq %%mm0, " #dst1 "          \n\t"\
        "movq %%mm1, " #dst2 "          \n\t"\
#define SIMPLE_CPY(src1, src2, dst1, dst2)\
   REAL_SIMPLE_CPY(src1, src2, dst1, dst2)
SIMPLE_CPY((%0)       , (%0, %2)       , (%1)       , (%1, %3))
SIMPLE_CPY((%0, %2, 2), (%%REGa, %2, 2), (%1, %3, 2), (%%REGd, %3, 2))
SIMPLE_CPY((%0, %2, 4), (%%REGa, %2, 4), (%1, %3, 4), (%%REGd, %3, 4))
        "lea (%%"REG_a",%2,4), %%"REG_a"        \n\t"
        "lea (%%"REG_d",%3,4), %%"REG_d"        \n\t"
SIMPLE_CPY((%%REGa, %2), (%%REGa, %2, 2), (%%REGd, %3), (%%REGd, %3, 2))
        : : "r" (src),
        "r" (dst),
        "r" ((x86_reg)srcStride),
        "r" ((x86_reg)dstStride)
        : "%"REG_a, "%"REG_d
    );
#else 
    for(i=0; i<8; i++)
        memcpy( &(dst[dstStride*i]),
                &(src[srcStride*i]), BLOCK_SIZE);
#endif 
    }
}
