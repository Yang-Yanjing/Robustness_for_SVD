static inline void RENAME(doVertLowPass)(uint8_t *src, int stride, PPContext *c)
{
#if TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW
    src+= stride*3;
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
        "psubusb %%mm0, %%mm2                   \n\t" 
        "pcmpeqb %%mm4, %%mm2                   \n\t" 
        "pand %%mm2, %%mm6                      \n\t"
        "pandn %%mm1, %%mm2                     \n\t"
        "por %%mm2, %%mm6                       \n\t"
        "movq (%0, %1, 8), %%mm5                \n\t"
        "lea (%0, %1, 4), %%"REG_a"             \n\t"
        "lea (%0, %1, 8), %%"REG_c"             \n\t"
        "sub %1, %%"REG_c"                      \n\t"
        "add %1, %0                             \n\t" 
        "movq (%0, %1, 8), %%mm7                \n\t"
        "movq %%mm5, %%mm1                      \n\t"
        "movq %%mm7, %%mm2                      \n\t"
        "psubusb %%mm7, %%mm5                   \n\t"
        "psubusb %%mm1, %%mm2                   \n\t"
        "por %%mm5, %%mm2                       \n\t" 
        "psubusb %%mm0, %%mm2                   \n\t" 
        "pcmpeqb %%mm4, %%mm2                   \n\t" 
        "pand %%mm2, %%mm7                      \n\t"
        "pandn %%mm1, %%mm2                     \n\t"
        "por %%mm2, %%mm7                       \n\t" 
        
        
        
        
        
        "movq (%0, %1), %%mm0                   \n\t" 
        "movq %%mm0, %%mm1                      \n\t" 
        PAVGB(%%mm6, %%mm0)                           
        PAVGB(%%mm6, %%mm0)                           
        "movq (%0, %1, 4), %%mm2                \n\t" 
        "movq %%mm2, %%mm5                      \n\t" 
        PAVGB((%%REGa), %%mm2)                        
        PAVGB((%0, %1, 2), %%mm2)                     
        "movq %%mm2, %%mm3                      \n\t" 
        "movq (%0), %%mm4                       \n\t" 
        PAVGB(%%mm4, %%mm3)                           
        PAVGB(%%mm0, %%mm3)                           
        "movq %%mm3, (%0)                       \n\t" 
        
        "movq %%mm1, %%mm0                      \n\t" 
        PAVGB(%%mm6, %%mm0)                           
        "movq %%mm4, %%mm3                      \n\t" 
        PAVGB((%0,%1,2), %%mm3)                       
        PAVGB((%%REGa,%1,2), %%mm5)                   
        PAVGB((%%REGa), %%mm5)                        
        PAVGB(%%mm5, %%mm3)                           
        PAVGB(%%mm0, %%mm3)                           
        "movq %%mm3, (%0,%1)                    \n\t" 
        
        PAVGB(%%mm4, %%mm6)                                   
        "movq (%%"REG_c"), %%mm0                \n\t" 
        PAVGB((%%REGa, %1, 2), %%mm0)                 
        "movq %%mm0, %%mm3                      \n\t" 
        PAVGB(%%mm1, %%mm0)                           
        PAVGB(%%mm6, %%mm0)                           
        PAVGB(%%mm2, %%mm0)                           
        "movq (%0, %1, 2), %%mm2                \n\t" 
        "movq %%mm0, (%0, %1, 2)                \n\t" 
        
        "movq (%%"REG_a", %1, 4), %%mm0         \n\t" 
        PAVGB((%%REGc), %%mm0)                        
        PAVGB(%%mm0, %%mm6)                           
        PAVGB(%%mm1, %%mm4)                           
        PAVGB(%%mm2, %%mm1)                           
        PAVGB(%%mm1, %%mm6)                           
        PAVGB(%%mm5, %%mm6)                           
        "movq (%%"REG_a"), %%mm5                \n\t" 
        "movq %%mm6, (%%"REG_a")                \n\t" 
        
        "movq (%%"REG_a", %1, 4), %%mm6         \n\t" 
        PAVGB(%%mm7, %%mm6)                           
        PAVGB(%%mm4, %%mm6)                           
        PAVGB(%%mm3, %%mm6)                           
        PAVGB(%%mm5, %%mm2)                           
        "movq (%0, %1, 4), %%mm4                \n\t" 
        PAVGB(%%mm4, %%mm2)                           
        PAVGB(%%mm2, %%mm6)                           
        "movq %%mm6, (%0, %1, 4)                \n\t" 
        
        PAVGB(%%mm7, %%mm1)                           
        PAVGB(%%mm4, %%mm5)                           
        PAVGB(%%mm5, %%mm0)                           
        "movq (%%"REG_a", %1, 2), %%mm6         \n\t" 
        PAVGB(%%mm6, %%mm1)                           
        PAVGB(%%mm0, %%mm1)                           
        "movq %%mm1, (%%"REG_a", %1, 2)         \n\t" 
        
        PAVGB((%%REGc), %%mm2)                        
        "movq (%%"REG_a", %1, 4), %%mm0         \n\t" 
        PAVGB(%%mm0, %%mm6)                           
        PAVGB(%%mm7, %%mm6)                           
        PAVGB(%%mm2, %%mm6)                           
        "movq %%mm6, (%%"REG_c")                \n\t" 
        
        PAVGB(%%mm7, %%mm5)                           
        PAVGB(%%mm7, %%mm5)                           
        PAVGB(%%mm3, %%mm0)                           
        PAVGB(%%mm0, %%mm5)                           
        "movq %%mm5, (%%"REG_a", %1, 4)         \n\t" 
        "sub %1, %0                             \n\t"
        :
        : "r" (src), "r" ((x86_reg)stride), "m" (c->pQPb)
        : "%"REG_a, "%"REG_c
    );
#else 
    const int l1= stride;
    const int l2= stride + l1;
    const int l3= stride + l2;
    const int l4= stride + l3;
    const int l5= stride + l4;
    const int l6= stride + l5;
    const int l7= stride + l6;
    const int l8= stride + l7;
    const int l9= stride + l8;
    int x;
    src+= stride*3;
    for(x=0; x<BLOCK_SIZE; x++){
        const int first= FFABS(src[0] - src[l1]) < c->QP ? src[0] : src[l1];
        const int last= FFABS(src[l8] - src[l9]) < c->QP ? src[l9] : src[l8];
        int sums[10];
        sums[0] = 4*first + src[l1] + src[l2] + src[l3] + 4;
        sums[1] = sums[0] - first  + src[l4];
        sums[2] = sums[1] - first  + src[l5];
        sums[3] = sums[2] - first  + src[l6];
        sums[4] = sums[3] - first  + src[l7];
        sums[5] = sums[4] - src[l1] + src[l8];
        sums[6] = sums[5] - src[l2] + last;
        sums[7] = sums[6] - src[l3] + last;
        sums[8] = sums[7] - src[l4] + last;
        sums[9] = sums[8] - src[l5] + last;
        src[l1]= (sums[0] + sums[2] + 2*src[l1])>>4;
        src[l2]= (sums[1] + sums[3] + 2*src[l2])>>4;
        src[l3]= (sums[2] + sums[4] + 2*src[l3])>>4;
        src[l4]= (sums[3] + sums[5] + 2*src[l4])>>4;
        src[l5]= (sums[4] + sums[6] + 2*src[l5])>>4;
        src[l6]= (sums[5] + sums[7] + 2*src[l6])>>4;
        src[l7]= (sums[6] + sums[8] + 2*src[l7])>>4;
        src[l8]= (sums[7] + sums[9] + 2*src[l8])>>4;
        src++;
    }
#endif 
}
