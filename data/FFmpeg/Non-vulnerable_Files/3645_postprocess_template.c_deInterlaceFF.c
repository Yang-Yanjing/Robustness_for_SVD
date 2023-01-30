static inline void RENAME(deInterlaceFF)(uint8_t src[], int stride, uint8_t *tmp)
{
#if TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW
    src+= stride*4;
    __asm__ volatile(
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_d"      \n\t"
        "pxor %%mm7, %%mm7                      \n\t"
        "movq (%2), %%mm0                       \n\t"


#define REAL_DEINT_FF(a,b,c,d)\
        "movq " #a ", %%mm1                     \n\t"\
        "movq " #b ", %%mm2                     \n\t"\
        "movq " #c ", %%mm3                     \n\t"\
        "movq " #d ", %%mm4                     \n\t"\
        PAVGB(%%mm3, %%mm1)                          \
        PAVGB(%%mm4, %%mm0)                          \
        "movq %%mm0, %%mm3                      \n\t"\
        "punpcklbw %%mm7, %%mm0                 \n\t"\
        "punpckhbw %%mm7, %%mm3                 \n\t"\
        "movq %%mm1, %%mm4                      \n\t"\
        "punpcklbw %%mm7, %%mm1                 \n\t"\
        "punpckhbw %%mm7, %%mm4                 \n\t"\
        "psllw $2, %%mm1                        \n\t"\
        "psllw $2, %%mm4                        \n\t"\
        "psubw %%mm0, %%mm1                     \n\t"\
        "psubw %%mm3, %%mm4                     \n\t"\
        "movq %%mm2, %%mm5                      \n\t"\
        "movq %%mm2, %%mm0                      \n\t"\
        "punpcklbw %%mm7, %%mm2                 \n\t"\
        "punpckhbw %%mm7, %%mm5                 \n\t"\
        "paddw %%mm2, %%mm1                     \n\t"\
        "paddw %%mm5, %%mm4                     \n\t"\
        "psraw $2, %%mm1                        \n\t"\
        "psraw $2, %%mm4                        \n\t"\
        "packuswb %%mm4, %%mm1                  \n\t"\
        "movq %%mm1, " #b "                     \n\t"\
#define DEINT_FF(a,b,c,d)  REAL_DEINT_FF(a,b,c,d)
DEINT_FF((%0)        , (%%REGa)       , (%%REGa, %1), (%%REGa, %1, 2))
DEINT_FF((%%REGa, %1), (%%REGa, %1, 2), (%0, %1, 4) , (%%REGd)       )
DEINT_FF((%0, %1, 4) , (%%REGd)       , (%%REGd, %1), (%%REGd, %1, 2))
DEINT_FF((%%REGd, %1), (%%REGd, %1, 2), (%0, %1, 8) , (%%REGd, %1, 4))
        "movq %%mm0, (%2)                       \n\t"
        : : "r" (src), "r" ((x86_reg)stride), "r"(tmp)
        : "%"REG_a, "%"REG_d
    );
#else 
    int x;
    src+= stride*4;
    for(x=0; x<8; x++){
        int t1= tmp[x];
        int t2= src[stride*1];
        src[stride*1]= av_clip_uint8((-t1 + 4*src[stride*0] + 2*t2 + 4*src[stride*2] - src[stride*3] + 4)>>3);
        t1= src[stride*4];
        src[stride*3]= av_clip_uint8((-t2 + 4*src[stride*2] + 2*t1 + 4*src[stride*4] - src[stride*5] + 4)>>3);
        t2= src[stride*6];
        src[stride*5]= av_clip_uint8((-t1 + 4*src[stride*4] + 2*t2 + 4*src[stride*6] - src[stride*7] + 4)>>3);
        t1= src[stride*8];
        src[stride*7]= av_clip_uint8((-t2 + 4*src[stride*6] + 2*t1 + 4*src[stride*8] - src[stride*9] + 4)>>3);
        tmp[x]= t1;
        src++;
    }
#endif 
}
