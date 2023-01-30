static av_always_inline void RENAME(do_a_deblock)(uint8_t *src, int step, int stride, const PPContext *c, int mode){
    int64_t dc_mask, eq_mask, both_masks;
    int64_t sums[10*8*2];
    src+= step*3; 
    
    __asm__ volatile(
        "movq %0, %%mm7                         \n\t"
        "movq %1, %%mm6                         \n\t"
        : : "m" (c->mmxDcOffset[c->nonBQP]),  "m" (c->mmxDcThreshold[c->nonBQP])
        );
    __asm__ volatile(
        "lea (%2, %3), %%"REG_a"                \n\t"


        "movq (%2), %%mm0                       \n\t"
        "movq (%%"REG_a"), %%mm1                \n\t"
        "movq %%mm1, %%mm3                      \n\t"
        "movq %%mm1, %%mm4                      \n\t"
        "psubb %%mm1, %%mm0                     \n\t" 
        "paddb %%mm7, %%mm0                     \n\t"
        "pcmpgtb %%mm6, %%mm0                   \n\t"
        "movq (%%"REG_a",%3), %%mm2             \n\t"
        PMAXUB(%%mm2, %%mm4)
        PMINUB(%%mm2, %%mm3, %%mm5)
        "psubb %%mm2, %%mm1                     \n\t"
        "paddb %%mm7, %%mm1                     \n\t"
        "pcmpgtb %%mm6, %%mm1                   \n\t"
        "paddb %%mm1, %%mm0                     \n\t"
        "movq (%%"REG_a", %3, 2), %%mm1         \n\t"
        PMAXUB(%%mm1, %%mm4)
        PMINUB(%%mm1, %%mm3, %%mm5)
        "psubb %%mm1, %%mm2                     \n\t"
        "paddb %%mm7, %%mm2                     \n\t"
        "pcmpgtb %%mm6, %%mm2                   \n\t"
        "paddb %%mm2, %%mm0                     \n\t"
        "lea (%%"REG_a", %3, 4), %%"REG_a"      \n\t"
        "movq (%2, %3, 4), %%mm2                \n\t"
        PMAXUB(%%mm2, %%mm4)
        PMINUB(%%mm2, %%mm3, %%mm5)
        "psubb %%mm2, %%mm1                     \n\t"
        "paddb %%mm7, %%mm1                     \n\t"
        "pcmpgtb %%mm6, %%mm1                   \n\t"
        "paddb %%mm1, %%mm0                     \n\t"
        "movq (%%"REG_a"), %%mm1                \n\t"
        PMAXUB(%%mm1, %%mm4)
        PMINUB(%%mm1, %%mm3, %%mm5)
        "psubb %%mm1, %%mm2                     \n\t"
        "paddb %%mm7, %%mm2                     \n\t"
        "pcmpgtb %%mm6, %%mm2                   \n\t"
        "paddb %%mm2, %%mm0                     \n\t"
        "movq (%%"REG_a", %3), %%mm2            \n\t"
        PMAXUB(%%mm2, %%mm4)
        PMINUB(%%mm2, %%mm3, %%mm5)
        "psubb %%mm2, %%mm1                     \n\t"
        "paddb %%mm7, %%mm1                     \n\t"
        "pcmpgtb %%mm6, %%mm1                   \n\t"
        "paddb %%mm1, %%mm0                     \n\t"
        "movq (%%"REG_a", %3, 2), %%mm1         \n\t"
        PMAXUB(%%mm1, %%mm4)
        PMINUB(%%mm1, %%mm3, %%mm5)
        "psubb %%mm1, %%mm2                     \n\t"
        "paddb %%mm7, %%mm2                     \n\t"
        "pcmpgtb %%mm6, %%mm2                   \n\t"
        "paddb %%mm2, %%mm0                     \n\t"
        "movq (%2, %3, 8), %%mm2                \n\t"
        PMAXUB(%%mm2, %%mm4)
        PMINUB(%%mm2, %%mm3, %%mm5)
        "psubb %%mm2, %%mm1                     \n\t"
        "paddb %%mm7, %%mm1                     \n\t"
        "pcmpgtb %%mm6, %%mm1                   \n\t"
        "paddb %%mm1, %%mm0                     \n\t"
        "movq (%%"REG_a", %3, 4), %%mm1         \n\t"
        "psubb %%mm1, %%mm2                     \n\t"
        "paddb %%mm7, %%mm2                     \n\t"
        "pcmpgtb %%mm6, %%mm2                   \n\t"
        "paddb %%mm2, %%mm0                     \n\t"
        "psubusb %%mm3, %%mm4                   \n\t"
        "pxor %%mm6, %%mm6                      \n\t"
        "movq %4, %%mm7                         \n\t" 
        "paddusb %%mm7, %%mm7                   \n\t" 
        "psubusb %%mm4, %%mm7                   \n\t" 
        "pcmpeqb %%mm6, %%mm7                   \n\t" 
        "pcmpeqb %%mm6, %%mm7                   \n\t" 
        "movq %%mm7, %1                         \n\t"
        "movq %5, %%mm7                         \n\t"
        "punpcklbw %%mm7, %%mm7                 \n\t"
        "punpcklbw %%mm7, %%mm7                 \n\t"
        "punpcklbw %%mm7, %%mm7                 \n\t"
        "psubb %%mm0, %%mm6                     \n\t"
        "pcmpgtb %%mm7, %%mm6                   \n\t"
        "movq %%mm6, %0                         \n\t"
        : "=m" (eq_mask), "=m" (dc_mask)
        : "r" (src), "r" ((x86_reg)step), "m" (c->pQPb), "m"(c->ppMode.flatnessThreshold)
        : "%"REG_a
    );
    both_masks = dc_mask & eq_mask;
    if(both_masks){
        x86_reg offset= -8*step;
        int64_t *temp_sums= sums;
        __asm__ volatile(
            "movq %2, %%mm0                         \n\t"  
            "pxor %%mm4, %%mm4                      \n\t"
            "movq (%0), %%mm6                       \n\t"
            "movq (%0, %1), %%mm5                   \n\t"
            "movq %%mm5, %%mm1                      \n\t"
            "movq %%mm6, %%mm2                      \n\t"
            "psubusb %%mm6, %%mm5                   \n\t"
            "psubusb %%mm1, %%mm2                   \n\t"
            "por %%mm5, %%mm2                       \n\t" 
            "psubusb %%mm2, %%mm0                   \n\t" 
            "pcmpeqb %%mm4, %%mm0                   \n\t" 
            "pxor %%mm6, %%mm1                      \n\t"
            "pand %%mm0, %%mm1                      \n\t"
            "pxor %%mm1, %%mm6                      \n\t"
            
            "movq (%0, %1, 8), %%mm5                \n\t"
            "add %1, %0                             \n\t" 
            "movq (%0, %1, 8), %%mm7                \n\t"
            "movq %%mm5, %%mm1                      \n\t"
            "movq %%mm7, %%mm2                      \n\t"
            "psubusb %%mm7, %%mm5                   \n\t"
            "psubusb %%mm1, %%mm2                   \n\t"
            "por %%mm5, %%mm2                       \n\t" 
            "movq %2, %%mm0                         \n\t"  
            "psubusb %%mm2, %%mm0                   \n\t" 
            "pcmpeqb %%mm4, %%mm0                   \n\t" 
            "pxor %%mm7, %%mm1                      \n\t"
            "pand %%mm0, %%mm1                      \n\t"
            "pxor %%mm1, %%mm7                      \n\t"
            "movq %%mm6, %%mm5                      \n\t"
            "punpckhbw %%mm4, %%mm6                 \n\t"
            "punpcklbw %%mm4, %%mm5                 \n\t"
            
            "movq %%mm5, %%mm0                      \n\t"
            "movq %%mm6, %%mm1                      \n\t"
            "psllw $2, %%mm0                        \n\t"
            "psllw $2, %%mm1                        \n\t"
            "paddw "MANGLE(w04)", %%mm0             \n\t"
            "paddw "MANGLE(w04)", %%mm1             \n\t"
#define NEXT\
            "movq (%0), %%mm2                       \n\t"\
            "movq (%0), %%mm3                       \n\t"\
            "add %1, %0                             \n\t"\
            "punpcklbw %%mm4, %%mm2                 \n\t"\
            "punpckhbw %%mm4, %%mm3                 \n\t"\
            "paddw %%mm2, %%mm0                     \n\t"\
            "paddw %%mm3, %%mm1                     \n\t"
#define PREV\
            "movq (%0), %%mm2                       \n\t"\
            "movq (%0), %%mm3                       \n\t"\
            "add %1, %0                             \n\t"\
            "punpcklbw %%mm4, %%mm2                 \n\t"\
            "punpckhbw %%mm4, %%mm3                 \n\t"\
            "psubw %%mm2, %%mm0                     \n\t"\
            "psubw %%mm3, %%mm1                     \n\t"
            NEXT 
            NEXT 
            NEXT 
            "movq %%mm0, (%3)                       \n\t"
            "movq %%mm1, 8(%3)                      \n\t"
            NEXT 
            "psubw %%mm5, %%mm0                     \n\t"
            "psubw %%mm6, %%mm1                     \n\t"
            "movq %%mm0, 16(%3)                     \n\t"
            "movq %%mm1, 24(%3)                     \n\t"
            NEXT 
            "psubw %%mm5, %%mm0                     \n\t"
            "psubw %%mm6, %%mm1                     \n\t"
            "movq %%mm0, 32(%3)                     \n\t"
            "movq %%mm1, 40(%3)                     \n\t"
            NEXT 
            "psubw %%mm5, %%mm0                     \n\t"
            "psubw %%mm6, %%mm1                     \n\t"
            "movq %%mm0, 48(%3)                     \n\t"
            "movq %%mm1, 56(%3)                     \n\t"
            NEXT 
            "psubw %%mm5, %%mm0                     \n\t"
            "psubw %%mm6, %%mm1                     \n\t"
            "movq %%mm0, 64(%3)                     \n\t"
            "movq %%mm1, 72(%3)                     \n\t"
            "movq %%mm7, %%mm6                      \n\t"
            "punpckhbw %%mm4, %%mm7                 \n\t"
            "punpcklbw %%mm4, %%mm6                 \n\t"
            NEXT 
            "mov %4, %0                             \n\t"
            "add %1, %0                             \n\t"
            PREV 
            "movq %%mm0, 80(%3)                     \n\t"
            "movq %%mm1, 88(%3)                     \n\t"
            PREV 
            "paddw %%mm6, %%mm0                     \n\t"
            "paddw %%mm7, %%mm1                     \n\t"
            "movq %%mm0, 96(%3)                     \n\t"
            "movq %%mm1, 104(%3)                    \n\t"
            PREV 
            "paddw %%mm6, %%mm0                     \n\t"
            "paddw %%mm7, %%mm1                     \n\t"
            "movq %%mm0, 112(%3)                    \n\t"
            "movq %%mm1, 120(%3)                    \n\t"
            PREV 
            "paddw %%mm6, %%mm0                     \n\t"
            "paddw %%mm7, %%mm1                     \n\t"
            "movq %%mm0, 128(%3)                    \n\t"
            "movq %%mm1, 136(%3)                    \n\t"
            PREV 
            "paddw %%mm6, %%mm0                     \n\t"
            "paddw %%mm7, %%mm1                     \n\t"
            "movq %%mm0, 144(%3)                    \n\t"
            "movq %%mm1, 152(%3)                    \n\t"
            "mov %4, %0                             \n\t" 
            : "+&r"(src)
            : "r" ((x86_reg)step), "m" (c->pQPb), "r"(sums), "g"(src)
              NAMED_CONSTRAINTS_ADD(w04)
        );
        src+= step; 
        __asm__ volatile(
            "movq %4, %%mm6                         \n\t"
            "pcmpeqb %%mm5, %%mm5                   \n\t"
            "pxor %%mm6, %%mm5                      \n\t"
            "pxor %%mm7, %%mm7                      \n\t"
            "1:                                     \n\t"
            "movq (%1), %%mm0                       \n\t"
            "movq 8(%1), %%mm1                      \n\t"
            "paddw 32(%1), %%mm0                    \n\t"
            "paddw 40(%1), %%mm1                    \n\t"
            "movq (%0, %3), %%mm2                   \n\t"
            "movq %%mm2, %%mm3                      \n\t"
            "movq %%mm2, %%mm4                      \n\t"
            "punpcklbw %%mm7, %%mm2                 \n\t"
            "punpckhbw %%mm7, %%mm3                 \n\t"
            "paddw %%mm2, %%mm0                     \n\t"
            "paddw %%mm3, %%mm1                     \n\t"
            "paddw %%mm2, %%mm0                     \n\t"
            "paddw %%mm3, %%mm1                     \n\t"
            "psrlw $4, %%mm0                        \n\t"
            "psrlw $4, %%mm1                        \n\t"
            "packuswb %%mm1, %%mm0                  \n\t"
            "pand %%mm6, %%mm0                      \n\t"
            "pand %%mm5, %%mm4                      \n\t"
            "por %%mm4, %%mm0                       \n\t"
            "movq %%mm0, (%0, %3)                   \n\t"
            "add $16, %1                            \n\t"
            "add %2, %0                             \n\t"
            " js 1b                                 \n\t"
            : "+r"(offset), "+r"(temp_sums)
            : "r" ((x86_reg)step), "r"(src - offset), "m"(both_masks)
        );
    }else
        src+= step; 
    if(eq_mask != -1LL){
        uint8_t *temp_src= src;
        DECLARE_ALIGNED(8, uint64_t, tmp)[4]; 
        __asm__ volatile(
            "pxor %%mm7, %%mm7                      \n\t"


            "movq (%0), %%mm0                       \n\t"
            "movq %%mm0, %%mm1                      \n\t"
            "punpcklbw %%mm7, %%mm0                 \n\t" 
            "punpckhbw %%mm7, %%mm1                 \n\t" 
            "movq (%0, %1), %%mm2                   \n\t"
            "lea (%0, %1, 2), %%"REG_a"             \n\t"
            "movq %%mm2, %%mm3                      \n\t"
            "punpcklbw %%mm7, %%mm2                 \n\t" 
            "punpckhbw %%mm7, %%mm3                 \n\t" 
            "movq (%%"REG_a"), %%mm4                \n\t"
            "movq %%mm4, %%mm5                      \n\t"
            "punpcklbw %%mm7, %%mm4                 \n\t" 
            "punpckhbw %%mm7, %%mm5                 \n\t" 
            "paddw %%mm0, %%mm0                     \n\t" 
            "paddw %%mm1, %%mm1                     \n\t" 
            "psubw %%mm4, %%mm2                     \n\t" 
            "psubw %%mm5, %%mm3                     \n\t" 
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "psllw $2, %%mm2                        \n\t" 
            "psllw $2, %%mm3                        \n\t" 
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "movq (%%"REG_a", %1), %%mm2            \n\t"
            "movq %%mm2, %%mm3                      \n\t"
            "punpcklbw %%mm7, %%mm2                 \n\t" 
            "punpckhbw %%mm7, %%mm3                 \n\t" 
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "movq %%mm0, (%4)                       \n\t" 
            "movq %%mm1, 8(%4)                      \n\t" 
            "movq (%%"REG_a", %1, 2), %%mm0         \n\t"
            "movq %%mm0, %%mm1                      \n\t"
            "punpcklbw %%mm7, %%mm0                 \n\t" 
            "punpckhbw %%mm7, %%mm1                 \n\t" 
            "psubw %%mm0, %%mm2                     \n\t" 
            "psubw %%mm1, %%mm3                     \n\t" 
            "movq %%mm2, 16(%4)                     \n\t" 
            "movq %%mm3, 24(%4)                     \n\t" 
            "paddw %%mm4, %%mm4                     \n\t" 
            "paddw %%mm5, %%mm5                     \n\t" 
            "psubw %%mm2, %%mm4                     \n\t" 
            "psubw %%mm3, %%mm5                     \n\t" 
            "lea (%%"REG_a", %1), %0                \n\t"
            "psllw $2, %%mm2                        \n\t" 
            "psllw $2, %%mm3                        \n\t" 
            "psubw %%mm2, %%mm4                     \n\t" 
            "psubw %%mm3, %%mm5                     \n\t" 

            "movq (%0, %1, 2), %%mm2                \n\t"
            "movq %%mm2, %%mm3                      \n\t"
            "punpcklbw %%mm7, %%mm2                 \n\t" 
            "punpckhbw %%mm7, %%mm3                 \n\t" 
            "psubw %%mm2, %%mm4                     \n\t" 
            "psubw %%mm3, %%mm5                     \n\t" 
            "psubw %%mm2, %%mm4                     \n\t" 
            "psubw %%mm3, %%mm5                     \n\t" 
            "movq (%%"REG_a", %1, 4), %%mm6         \n\t"
            "punpcklbw %%mm7, %%mm6                 \n\t" 
            "psubw %%mm6, %%mm2                     \n\t" 
            "movq (%%"REG_a", %1, 4), %%mm6         \n\t"
            "punpckhbw %%mm7, %%mm6                 \n\t" 
            "psubw %%mm6, %%mm3                     \n\t" 
            "paddw %%mm0, %%mm0                     \n\t" 
            "paddw %%mm1, %%mm1                     \n\t" 
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "psllw $2, %%mm2                        \n\t" 
            "psllw $2, %%mm3                        \n\t" 
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "movq (%0, %1, 4), %%mm2                \n\t"
            "movq %%mm2, %%mm3                      \n\t"
            "punpcklbw %%mm7, %%mm2                 \n\t" 
            "punpckhbw %%mm7, %%mm3                 \n\t" 
            "paddw %%mm2, %%mm2                     \n\t" 
            "paddw %%mm3, %%mm3                     \n\t" 
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "movq (%4), %%mm2                       \n\t" 
            "movq 8(%4), %%mm3                      \n\t" 
#if TEMPLATE_PP_MMXEXT
            "movq %%mm7, %%mm6                      \n\t" 
            "psubw %%mm0, %%mm6                     \n\t"
            "pmaxsw %%mm6, %%mm0                    \n\t" 
            "movq %%mm7, %%mm6                      \n\t" 
            "psubw %%mm1, %%mm6                     \n\t"
            "pmaxsw %%mm6, %%mm1                    \n\t" 
            "movq %%mm7, %%mm6                      \n\t" 
            "psubw %%mm2, %%mm6                     \n\t"
            "pmaxsw %%mm6, %%mm2                    \n\t" 
            "movq %%mm7, %%mm6                      \n\t" 
            "psubw %%mm3, %%mm6                     \n\t"
            "pmaxsw %%mm6, %%mm3                    \n\t" 
#else
            "movq %%mm7, %%mm6                      \n\t" 
            "pcmpgtw %%mm0, %%mm6                   \n\t"
            "pxor %%mm6, %%mm0                      \n\t"
            "psubw %%mm6, %%mm0                     \n\t" 
            "movq %%mm7, %%mm6                      \n\t" 
            "pcmpgtw %%mm1, %%mm6                   \n\t"
            "pxor %%mm6, %%mm1                      \n\t"
            "psubw %%mm6, %%mm1                     \n\t" 
            "movq %%mm7, %%mm6                      \n\t" 
            "pcmpgtw %%mm2, %%mm6                   \n\t"
            "pxor %%mm6, %%mm2                      \n\t"
            "psubw %%mm6, %%mm2                     \n\t" 
            "movq %%mm7, %%mm6                      \n\t" 
            "pcmpgtw %%mm3, %%mm6                   \n\t"
            "pxor %%mm6, %%mm3                      \n\t"
            "psubw %%mm6, %%mm3                     \n\t" 
#endif
#if TEMPLATE_PP_MMXEXT
            "pminsw %%mm2, %%mm0                    \n\t"
            "pminsw %%mm3, %%mm1                    \n\t"
#else
            "movq %%mm0, %%mm6                      \n\t"
            "psubusw %%mm2, %%mm6                   \n\t"
            "psubw %%mm6, %%mm0                     \n\t"
            "movq %%mm1, %%mm6                      \n\t"
            "psubusw %%mm3, %%mm6                   \n\t"
            "psubw %%mm6, %%mm1                     \n\t"
#endif
            "movd %2, %%mm2                         \n\t" 
            "punpcklbw %%mm7, %%mm2                 \n\t"
            "movq %%mm7, %%mm6                      \n\t" 
            "pcmpgtw %%mm4, %%mm6                   \n\t" 
            "pxor %%mm6, %%mm4                      \n\t"
            "psubw %%mm6, %%mm4                     \n\t" 
            "pcmpgtw %%mm5, %%mm7                   \n\t" 
            "pxor %%mm7, %%mm5                      \n\t"
            "psubw %%mm7, %%mm5                     \n\t" 

            "psllw $3, %%mm2                        \n\t" 
            "movq %%mm2, %%mm3                      \n\t" 
            "pcmpgtw %%mm4, %%mm2                   \n\t"
            "pcmpgtw %%mm5, %%mm3                   \n\t"
            "pand %%mm2, %%mm4                      \n\t"
            "pand %%mm3, %%mm5                      \n\t"
            "psubusw %%mm0, %%mm4                   \n\t" 
            "psubusw %%mm1, %%mm5                   \n\t" 
            "movq "MANGLE(w05)", %%mm2              \n\t" 
            "pmullw %%mm2, %%mm4                    \n\t"
            "pmullw %%mm2, %%mm5                    \n\t"
            "movq "MANGLE(w20)", %%mm2              \n\t" 
            "paddw %%mm2, %%mm4                     \n\t"
            "paddw %%mm2, %%mm5                     \n\t"
            "psrlw $6, %%mm4                        \n\t"
            "psrlw $6, %%mm5                        \n\t"
            "movq 16(%4), %%mm0                     \n\t" 
            "movq 24(%4), %%mm1                     \n\t" 
            "pxor %%mm2, %%mm2                      \n\t"
            "pxor %%mm3, %%mm3                      \n\t"
            "pcmpgtw %%mm0, %%mm2                   \n\t" 
            "pcmpgtw %%mm1, %%mm3                   \n\t" 
            "pxor %%mm2, %%mm0                      \n\t"
            "pxor %%mm3, %%mm1                      \n\t"
            "psubw %%mm2, %%mm0                     \n\t" 
            "psubw %%mm3, %%mm1                     \n\t" 
            "psrlw $1, %%mm0                        \n\t" 
            "psrlw $1, %%mm1                        \n\t" 
            "pxor %%mm6, %%mm2                      \n\t"
            "pxor %%mm7, %%mm3                      \n\t"
            "pand %%mm2, %%mm4                      \n\t"
            "pand %%mm3, %%mm5                      \n\t"
#if TEMPLATE_PP_MMXEXT
            "pminsw %%mm0, %%mm4                    \n\t"
            "pminsw %%mm1, %%mm5                    \n\t"
#else
            "movq %%mm4, %%mm2                      \n\t"
            "psubusw %%mm0, %%mm2                   \n\t"
            "psubw %%mm2, %%mm4                     \n\t"
            "movq %%mm5, %%mm2                      \n\t"
            "psubusw %%mm1, %%mm2                   \n\t"
            "psubw %%mm2, %%mm5                     \n\t"
#endif
            "pxor %%mm6, %%mm4                      \n\t"
            "pxor %%mm7, %%mm5                      \n\t"
            "psubw %%mm6, %%mm4                     \n\t"
            "psubw %%mm7, %%mm5                     \n\t"
            "packsswb %%mm5, %%mm4                  \n\t"
            "movq %3, %%mm1                         \n\t"
            "pandn %%mm4, %%mm1                     \n\t"
            "movq (%0), %%mm0                       \n\t"
            "paddb   %%mm1, %%mm0                   \n\t"
            "movq %%mm0, (%0)                       \n\t"
            "movq (%0, %1), %%mm0                   \n\t"
            "psubb %%mm1, %%mm0                     \n\t"
            "movq %%mm0, (%0, %1)                   \n\t"
            : "+r" (temp_src)
            : "r" ((x86_reg)step), "m" (c->pQPb), "m"(eq_mask), "r"(tmp)
              NAMED_CONSTRAINTS_ADD(w05,w20)
            : "%"REG_a
        );
    }






}
