static inline void RENAME(vertX1Filter)(uint8_t *src, int stride, PPContext *co)
{
#if TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW
    src+= stride*3;
    __asm__ volatile(
        "pxor %%mm7, %%mm7                      \n\t" 
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_c"      \n\t"


        "movq (%%"REG_a", %1, 2), %%mm0         \n\t" 
        "movq (%0, %1, 4), %%mm1                \n\t" 
        "movq %%mm1, %%mm2                      \n\t" 
        "psubusb %%mm0, %%mm1                   \n\t"
        "psubusb %%mm2, %%mm0                   \n\t"
        "por %%mm1, %%mm0                       \n\t" 
        "movq (%%"REG_c"), %%mm3                \n\t" 
        "movq (%%"REG_c", %1), %%mm4            \n\t" 
        "movq %%mm3, %%mm5                      \n\t" 
        "psubusb %%mm4, %%mm3                   \n\t"
        "psubusb %%mm5, %%mm4                   \n\t"
        "por %%mm4, %%mm3                       \n\t" 
        PAVGB(%%mm3, %%mm0)                           
        "movq %%mm2, %%mm1                      \n\t" 
        "psubusb %%mm5, %%mm2                   \n\t"
        "movq %%mm2, %%mm4                      \n\t"
        "pcmpeqb %%mm7, %%mm2                   \n\t" 
        "psubusb %%mm1, %%mm5                   \n\t"
        "por %%mm5, %%mm4                       \n\t" 
        "psubusb %%mm0, %%mm4                   \n\t" 
        "movq %%mm4, %%mm3                      \n\t" 
        "movq %2, %%mm0                         \n\t"
        "paddusb %%mm0, %%mm0                   \n\t"
        "psubusb %%mm0, %%mm4                   \n\t"
        "pcmpeqb %%mm7, %%mm4                   \n\t" 
        "psubusb "MANGLE(b01)", %%mm3           \n\t"
        "pand %%mm4, %%mm3                      \n\t" 
        PAVGB(%%mm7, %%mm3)                           
        "movq %%mm3, %%mm1                      \n\t" 
        PAVGB(%%mm7, %%mm3)                           
        PAVGB(%%mm1, %%mm3)                           
        "movq (%0, %1, 4), %%mm0                \n\t" 
        "pxor %%mm2, %%mm0                      \n\t" 
        "psubusb %%mm3, %%mm0                   \n\t"
        "pxor %%mm2, %%mm0                      \n\t"
        "movq %%mm0, (%0, %1, 4)                \n\t" 
        "movq (%%"REG_c"), %%mm0                \n\t" 
        "pxor %%mm2, %%mm0                      \n\t" 
        "paddusb %%mm3, %%mm0                   \n\t"
        "pxor %%mm2, %%mm0                      \n\t"
        "movq %%mm0, (%%"REG_c")                \n\t" 
        PAVGB(%%mm7, %%mm1)                           
        "movq (%%"REG_a", %1, 2), %%mm0         \n\t" 
        "pxor %%mm2, %%mm0                      \n\t" 
        "psubusb %%mm1, %%mm0                   \n\t"
        "pxor %%mm2, %%mm0                      \n\t"
        "movq %%mm0, (%%"REG_a", %1, 2)         \n\t" 
        "movq (%%"REG_c", %1), %%mm0            \n\t" 
        "pxor %%mm2, %%mm0                      \n\t" 
        "paddusb %%mm1, %%mm0                   \n\t"
        "pxor %%mm2, %%mm0                      \n\t"
        "movq %%mm0, (%%"REG_c", %1)            \n\t" 
        PAVGB(%%mm7, %%mm1)                           
        "movq (%%"REG_a", %1), %%mm0            \n\t" 
        "pxor %%mm2, %%mm0                      \n\t" 
        "psubusb %%mm1, %%mm0                   \n\t"
        "pxor %%mm2, %%mm0                      \n\t"
        "movq %%mm0, (%%"REG_a", %1)            \n\t" 
        "movq (%%"REG_c", %1, 2), %%mm0         \n\t" 
        "pxor %%mm2, %%mm0                      \n\t" 
        "paddusb %%mm1, %%mm0                   \n\t"
        "pxor %%mm2, %%mm0                      \n\t"
        "movq %%mm0, (%%"REG_c", %1, 2)         \n\t" 
        :
        : "r" (src), "r" ((x86_reg)stride), "m" (co->pQPb)
          NAMED_CONSTRAINTS_ADD(b01)
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


    int x;
    src+= stride*3;
    for(x=0; x<BLOCK_SIZE; x++){
        int a= src[l3] - src[l4];
        int b= src[l4] - src[l5];
        int c= src[l5] - src[l6];
        int d= FFABS(b) - ((FFABS(a) + FFABS(c))>>1);
        d= FFMAX(d, 0);
        if(d < co->QP*2){
            int v = d * FFSIGN(-b);
            src[l2] +=v>>3;
            src[l3] +=v>>2;
            src[l4] +=(3*v)>>3;
            src[l5] -=(3*v)>>3;
            src[l6] -=v>>2;
            src[l7] -=v>>3;
        }
        src++;
    }
#endif 
}
