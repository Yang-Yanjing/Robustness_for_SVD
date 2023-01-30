static inline int RENAME(vertClassify)(const uint8_t src[], int stride, PPContext *c){
    int numEq= 0, dcOk;
    src+= stride*4; 
    __asm__ volatile(
        "movq %0, %%mm7                         \n\t"
        "movq %1, %%mm6                         \n\t"
        : : "m" (c->mmxDcOffset[c->nonBQP]),  "m" (c->mmxDcThreshold[c->nonBQP])
        );
    __asm__ volatile(
        "lea (%2, %3), %%"REG_a"                \n\t"


        "movq (%2), %%mm0                       \n\t"
        "movq (%%"REG_a"), %%mm1                \n\t"
        "movq %%mm0, %%mm3                      \n\t"
        "movq %%mm0, %%mm4                      \n\t"
        PMAXUB(%%mm1, %%mm4)
        PMINUB(%%mm1, %%mm3, %%mm5)
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
        "psubusb %%mm3, %%mm4                   \n\t"
        "                                       \n\t"
#if TEMPLATE_PP_MMXEXT
        "pxor %%mm7, %%mm7                      \n\t"
        "psadbw %%mm7, %%mm0                    \n\t"
#else
        "movq %%mm0, %%mm1                      \n\t"
        "psrlw $8, %%mm0                        \n\t"
        "paddb %%mm1, %%mm0                     \n\t"
        "movq %%mm0, %%mm1                      \n\t"
        "psrlq $16, %%mm0                       \n\t"
        "paddb %%mm1, %%mm0                     \n\t"
        "movq %%mm0, %%mm1                      \n\t"
        "psrlq $32, %%mm0                       \n\t"
        "paddb %%mm1, %%mm0                     \n\t"
#endif
        "movq %4, %%mm7                         \n\t" 
        "paddusb %%mm7, %%mm7                   \n\t" 
        "psubusb %%mm7, %%mm4                   \n\t" 
        "packssdw %%mm4, %%mm4                  \n\t"
        "movd %%mm0, %0                         \n\t"
        "movd %%mm4, %1                         \n\t"
        : "=r" (numEq), "=r" (dcOk)
        : "r" (src), "r" ((x86_reg)stride), "m" (c->pQPb)
        : "%"REG_a
        );
    numEq= (-numEq) &0xFF;
    if(numEq > c->ppMode.flatnessThreshold){
        if(dcOk) return 0;
        else     return 1;
    }else{
        return 2;
    }
}
