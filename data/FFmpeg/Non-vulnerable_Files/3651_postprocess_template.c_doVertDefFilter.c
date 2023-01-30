static inline void RENAME(doVertDefFilter)(uint8_t src[], int stride, PPContext *c)
{
#if TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW













    src+= stride*4;
    __asm__ volatile(
#if 0 
        "pxor %%mm7, %%mm7                      \n\t" 
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_c"      \n\t"



        "movq (%0, %1, 2), %%mm0                \n\t" 
        "movq (%0), %%mm1                       \n\t" 
        "movq %%mm0, %%mm2                      \n\t" 
        PAVGB(%%mm7, %%mm0)                           
        PAVGB(%%mm1, %%mm0)                           
        PAVGB(%%mm2, %%mm0)                           
        "movq (%%"REG_a"), %%mm1                \n\t" 
        "movq (%%"REG_a", %1, 2), %%mm3         \n\t" 
        "movq %%mm1, %%mm4                      \n\t" 
        PAVGB(%%mm7, %%mm1)                           
        PAVGB(%%mm3, %%mm1)                           
        PAVGB(%%mm4, %%mm1)                           
        "movq %%mm0, %%mm4                      \n\t" 
        "psubusb %%mm1, %%mm0                   \n\t"
        "psubusb %%mm4, %%mm1                   \n\t"
        "por %%mm0, %%mm1                       \n\t" 

        "movq (%0, %1, 4), %%mm0                \n\t" 
        "movq %%mm0, %%mm4                      \n\t" 
        PAVGB(%%mm7, %%mm0)                           
        PAVGB(%%mm2, %%mm0)                           
        PAVGB(%%mm4, %%mm0)                           
        "movq (%%"REG_c"), %%mm2                \n\t" 
        "movq %%mm3, %%mm5                      \n\t" 
        PAVGB(%%mm7, %%mm3)                           
        PAVGB(%%mm2, %%mm3)                           
        PAVGB(%%mm5, %%mm3)                           
        "movq %%mm0, %%mm6                      \n\t" 
        "psubusb %%mm3, %%mm0                   \n\t"
        "psubusb %%mm6, %%mm3                   \n\t"
        "por %%mm0, %%mm3                       \n\t" 
        "pcmpeqb %%mm7, %%mm0                   \n\t" 

        "movq (%%"REG_c", %1), %%mm6            \n\t" 
        "movq %%mm6, %%mm5                      \n\t" 
        PAVGB(%%mm7, %%mm6)                           
        PAVGB(%%mm4, %%mm6)                           
        PAVGB(%%mm5, %%mm6)                           
        "movq (%%"REG_c", %1, 2), %%mm5         \n\t" 
        "movq %%mm2, %%mm4                      \n\t" 
        PAVGB(%%mm7, %%mm2)                           
        PAVGB(%%mm5, %%mm2)                           
        PAVGB(%%mm4, %%mm2)                           
        "movq %%mm6, %%mm4                      \n\t" 
        "psubusb %%mm2, %%mm6                   \n\t"
        "psubusb %%mm4, %%mm2                   \n\t"
        "por %%mm6, %%mm2                       \n\t" 

        PMINUB(%%mm2, %%mm1, %%mm4)                   
        "movq %2, %%mm4                         \n\t" 
        "paddusb "MANGLE(b01)", %%mm4           \n\t"
        "pcmpgtb %%mm3, %%mm4                   \n\t" 
        "psubusb %%mm1, %%mm3                   \n\t" 
        "pand %%mm4, %%mm3                      \n\t"
        "movq %%mm3, %%mm1                      \n\t"

        PAVGB(%%mm7, %%mm3)
        PAVGB(%%mm7, %%mm3)
        "paddusb %%mm1, %%mm3                   \n\t"

        "movq (%%"REG_a", %1, 2), %%mm6         \n\t" 
        "movq (%0, %1, 4), %%mm5                \n\t" 
        "movq (%0, %1, 4), %%mm4                \n\t" 
        "psubusb %%mm6, %%mm5                   \n\t"
        "psubusb %%mm4, %%mm6                   \n\t"
        "por %%mm6, %%mm5                       \n\t" 
        "pcmpeqb %%mm7, %%mm6                   \n\t" 
        "pxor %%mm6, %%mm0                      \n\t"
        "pand %%mm0, %%mm3                      \n\t"
        PMINUB(%%mm5, %%mm3, %%mm0)
        "psubusb "MANGLE(b01)", %%mm3           \n\t"
        PAVGB(%%mm7, %%mm3)
        "movq (%%"REG_a", %1, 2), %%mm0         \n\t"
        "movq (%0, %1, 4), %%mm2                \n\t"
        "pxor %%mm6, %%mm0                      \n\t"
        "pxor %%mm6, %%mm2                      \n\t"
        "psubb %%mm3, %%mm0                     \n\t"
        "paddb %%mm3, %%mm2                     \n\t"
        "pxor %%mm6, %%mm0                      \n\t"
        "pxor %%mm6, %%mm2                      \n\t"
        "movq %%mm0, (%%"REG_a", %1, 2)         \n\t"
        "movq %%mm2, (%0, %1, 4)                \n\t"
#endif 
        "lea (%0, %1), %%"REG_a"                \n\t"
        "pcmpeqb %%mm6, %%mm6                   \n\t" 



        "movq (%%"REG_a", %1, 2), %%mm1         \n\t" 
        "movq (%0, %1, 4), %%mm0                \n\t" 
        "pxor %%mm6, %%mm1                      \n\t" 
        PAVGB(%%mm1, %%mm0)                           

        "movq (%%"REG_a", %1, 4), %%mm2         \n\t" 
        "movq (%%"REG_a", %1), %%mm3            \n\t" 
        "pxor %%mm6, %%mm2                      \n\t" 
        "movq %%mm2, %%mm5                      \n\t" 
        "movq "MANGLE(b80)", %%mm4              \n\t" 
        "lea (%%"REG_a", %1, 4), %%"REG_c"      \n\t"
        PAVGB(%%mm3, %%mm2)                           
        PAVGB(%%mm0, %%mm4)                           
        PAVGB(%%mm2, %%mm4)                           
        PAVGB(%%mm0, %%mm4)                           

        "movq (%%"REG_a"), %%mm2                \n\t" 
        "pxor %%mm6, %%mm2                      \n\t" 
        PAVGB(%%mm3, %%mm2)                           
        PAVGB((%0), %%mm1)                            
        "movq "MANGLE(b80)", %%mm3              \n\t" 
        PAVGB(%%mm2, %%mm3)                           
        PAVGB(%%mm1, %%mm3)                           
        PAVGB(%%mm2, %%mm3)                           

        PAVGB((%%REGc, %1), %%mm5)                    
        "movq (%%"REG_c", %1, 2), %%mm1         \n\t" 
        "pxor %%mm6, %%mm1                      \n\t" 
        PAVGB((%0, %1, 4), %%mm1)                     
        "movq "MANGLE(b80)", %%mm2              \n\t" 
        PAVGB(%%mm5, %%mm2)                           
        PAVGB(%%mm1, %%mm2)                           
        PAVGB(%%mm5, %%mm2)                           

        "movq "MANGLE(b00)", %%mm1              \n\t" 
        "movq "MANGLE(b00)", %%mm5              \n\t" 
        "psubb %%mm2, %%mm1                     \n\t" 
        "psubb %%mm3, %%mm5                     \n\t" 
        PMAXUB(%%mm1, %%mm2)                          
        PMAXUB(%%mm5, %%mm3)                          
        PMINUB(%%mm2, %%mm3, %%mm1)                   

        "movq "MANGLE(b00)", %%mm7              \n\t" 
        "movq %2, %%mm2                         \n\t" 
        PAVGB(%%mm6, %%mm2)                           
        "psubb %%mm6, %%mm2                     \n\t"
        "movq %%mm4, %%mm1                      \n\t"
        "pcmpgtb %%mm7, %%mm1                   \n\t" 
        "pxor %%mm1, %%mm4                      \n\t"
        "psubb %%mm1, %%mm4                     \n\t" 
        "pcmpgtb %%mm4, %%mm2                   \n\t" 
        "psubusb %%mm3, %%mm4                   \n\t" 

        "movq %%mm4, %%mm3                      \n\t" 
        "psubusb "MANGLE(b01)", %%mm4           \n\t"
        PAVGB(%%mm7, %%mm4)                           
        PAVGB(%%mm7, %%mm4)                           
        "paddb %%mm3, %%mm4                     \n\t" 
        "pand %%mm2, %%mm4                      \n\t"
        "movq "MANGLE(b80)", %%mm5              \n\t" 
        "psubb %%mm0, %%mm5                     \n\t" 
        "paddsb %%mm6, %%mm5                    \n\t" 
        "pcmpgtb %%mm5, %%mm7                   \n\t" 
        "pxor %%mm7, %%mm5                      \n\t"
        PMINUB(%%mm5, %%mm4, %%mm3)                   
        "pxor %%mm1, %%mm7                      \n\t" 
        "pand %%mm7, %%mm4                      \n\t"
        "movq (%%"REG_a", %1, 2), %%mm0         \n\t"
        "movq (%0, %1, 4), %%mm2                \n\t"
        "pxor %%mm1, %%mm0                      \n\t"
        "pxor %%mm1, %%mm2                      \n\t"
        "paddb %%mm4, %%mm0                     \n\t"
        "psubb %%mm4, %%mm2                     \n\t"
        "pxor %%mm1, %%mm0                      \n\t"
        "pxor %%mm1, %%mm2                      \n\t"
        "movq %%mm0, (%%"REG_a", %1, 2)         \n\t"
        "movq %%mm2, (%0, %1, 4)                \n\t"
        :
        : "r" (src), "r" ((x86_reg)stride), "m" (c->pQPb)
          NAMED_CONSTRAINTS_ADD(b80,b00,b01)
        : "%"REG_a, "%"REG_c
    );


















































#elif TEMPLATE_PP_MMX
    DECLARE_ALIGNED(8, uint64_t, tmp)[4]; 
    src+= stride*4;
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
        "movq %%mm0, (%3)                       \n\t" 
        "movq %%mm1, 8(%3)                      \n\t" 
        "movq (%%"REG_a", %1, 2), %%mm0         \n\t"
        "movq %%mm0, %%mm1                      \n\t"
        "punpcklbw %%mm7, %%mm0                 \n\t" 
        "punpckhbw %%mm7, %%mm1                 \n\t" 
        "psubw %%mm0, %%mm2                     \n\t" 
        "psubw %%mm1, %%mm3                     \n\t" 
        "movq %%mm2, 16(%3)                     \n\t" 
        "movq %%mm3, 24(%3)                     \n\t" 
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
        "movq (%3), %%mm2                       \n\t" 
        "movq 8(%3), %%mm3                      \n\t" 
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
        "movq 16(%3), %%mm0                     \n\t" 
        "movq 24(%3), %%mm1                     \n\t" 
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
        "movq (%0), %%mm0                       \n\t"
        "paddb   %%mm4, %%mm0                   \n\t"
        "movq %%mm0, (%0)                       \n\t"
        "movq (%0, %1), %%mm0                   \n\t"
        "psubb %%mm4, %%mm0                     \n\t"
        "movq %%mm0, (%0, %1)                   \n\t"
        : "+r" (src)
        : "r" ((x86_reg)stride), "m" (c->pQPb), "r"(tmp)
          NAMED_CONSTRAINTS_ADD(w05,w20)
        : "%"REG_a
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

    int x;
    src+= stride*3;
    for(x=0; x<BLOCK_SIZE; x++){
        const int middleEnergy= 5*(src[l5] - src[l4]) + 2*(src[l3] - src[l6]);
        if(FFABS(middleEnergy) < 8*c->QP){
            const int q=(src[l4] - src[l5])/2;
            const int leftEnergy=  5*(src[l3] - src[l2]) + 2*(src[l1] - src[l4]);
            const int rightEnergy= 5*(src[l7] - src[l6]) + 2*(src[l5] - src[l8]);
            int d= FFABS(middleEnergy) - FFMIN( FFABS(leftEnergy), FFABS(rightEnergy) );
            d= FFMAX(d, 0);
            d= (5*d + 32) >> 6;
            d*= FFSIGN(-middleEnergy);
            if(q>0){
                d = FFMAX(d, 0);
                d = FFMIN(d, q);
            }else{
                d = FFMIN(d, 0);
                d = FFMAX(d, q);
            }
            src[l4]-= d;
            src[l5]+= d;
        }
        src++;
    }
#endif 
}
