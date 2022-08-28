static inline void RENAME(deInterlaceL5)(uint8_t src[], int stride, uint8_t *tmp, uint8_t *tmp2)
{
#if (TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW) && HAVE_6REGS
    src+= stride*4;
    __asm__ volatile(
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_d"      \n\t"
        "pxor %%mm7, %%mm7                      \n\t"
        "movq (%2), %%mm0                       \n\t"
        "movq (%3), %%mm1                       \n\t"


#define REAL_DEINT_L5(t1,t2,a,b,c)\
        "movq " #a ", %%mm2                     \n\t"\
        "movq " #b ", %%mm3                     \n\t"\
        "movq " #c ", %%mm4                     \n\t"\
        PAVGB(t2, %%mm3)                             \
        PAVGB(t1, %%mm4)                             \
        "movq %%mm2, %%mm5                      \n\t"\
        "movq %%mm2, " #t1 "                    \n\t"\
        "punpcklbw %%mm7, %%mm2                 \n\t"\
        "punpckhbw %%mm7, %%mm5                 \n\t"\
        "movq %%mm2, %%mm6                      \n\t"\
        "paddw %%mm2, %%mm2                     \n\t"\
        "paddw %%mm6, %%mm2                     \n\t"\
        "movq %%mm5, %%mm6                      \n\t"\
        "paddw %%mm5, %%mm5                     \n\t"\
        "paddw %%mm6, %%mm5                     \n\t"\
        "movq %%mm3, %%mm6                      \n\t"\
        "punpcklbw %%mm7, %%mm3                 \n\t"\
        "punpckhbw %%mm7, %%mm6                 \n\t"\
        "paddw %%mm3, %%mm3                     \n\t"\
        "paddw %%mm6, %%mm6                     \n\t"\
        "paddw %%mm3, %%mm2                     \n\t"\
        "paddw %%mm6, %%mm5                     \n\t"\
        "movq %%mm4, %%mm6                      \n\t"\
        "punpcklbw %%mm7, %%mm4                 \n\t"\
        "punpckhbw %%mm7, %%mm6                 \n\t"\
        "psubw %%mm4, %%mm2                     \n\t"\
        "psubw %%mm6, %%mm5                     \n\t"\
        "psraw $2, %%mm2                        \n\t"\
        "psraw $2, %%mm5                        \n\t"\
        "packuswb %%mm5, %%mm2                  \n\t"\
        "movq %%mm2, " #a "                     \n\t"\
#define DEINT_L5(t1,t2,a,b,c)  REAL_DEINT_L5(t1,t2,a,b,c)
DEINT_L5(%%mm0, %%mm1, (%0)           , (%%REGa)       , (%%REGa, %1)   )
DEINT_L5(%%mm1, %%mm0, (%%REGa)       , (%%REGa, %1)   , (%%REGa, %1, 2))
DEINT_L5(%%mm0, %%mm1, (%%REGa, %1)   , (%%REGa, %1, 2), (%0, %1, 4)   )
DEINT_L5(%%mm1, %%mm0, (%%REGa, %1, 2), (%0, %1, 4)    , (%%REGd)       )
DEINT_L5(%%mm0, %%mm1, (%0, %1, 4)    , (%%REGd)       , (%%REGd, %1)   )
DEINT_L5(%%mm1, %%mm0, (%%REGd)       , (%%REGd, %1)   , (%%REGd, %1, 2))
DEINT_L5(%%mm0, %%mm1, (%%REGd, %1)   , (%%REGd, %1, 2), (%0, %1, 8)   )
DEINT_L5(%%mm1, %%mm0, (%%REGd, %1, 2), (%0, %1, 8)    , (%%REGd, %1, 4))
        "movq %%mm0, (%2)                       \n\t"
        "movq %%mm1, (%3)                       \n\t"
        : : "r" (src), "r" ((x86_reg)stride), "r"(tmp), "r"(tmp2)
        : "%"REG_a, "%"REG_d
    );
#else 
    int x;
    src+= stride*4;
    for(x=0; x<8; x++){
        int t1= tmp[x];
        int t2= tmp2[x];
        int t3= src[0];
        src[stride*0]= av_clip_uint8((-(t1 + src[stride*2]) + 2*(t2 + src[stride*1]) + 6*t3 + 4)>>3);
        t1= src[stride*1];
        src[stride*1]= av_clip_uint8((-(t2 + src[stride*3]) + 2*(t3 + src[stride*2]) + 6*t1 + 4)>>3);
        t2= src[stride*2];
        src[stride*2]= av_clip_uint8((-(t3 + src[stride*4]) + 2*(t1 + src[stride*3]) + 6*t2 + 4)>>3);
        t3= src[stride*3];
        src[stride*3]= av_clip_uint8((-(t1 + src[stride*5]) + 2*(t2 + src[stride*4]) + 6*t3 + 4)>>3);
        t1= src[stride*4];
        src[stride*4]= av_clip_uint8((-(t2 + src[stride*6]) + 2*(t3 + src[stride*5]) + 6*t1 + 4)>>3);
        t2= src[stride*5];
        src[stride*5]= av_clip_uint8((-(t3 + src[stride*7]) + 2*(t1 + src[stride*6]) + 6*t2 + 4)>>3);
        t3= src[stride*6];
        src[stride*6]= av_clip_uint8((-(t1 + src[stride*8]) + 2*(t2 + src[stride*7]) + 6*t3 + 4)>>3);
        t1= src[stride*7];
        src[stride*7]= av_clip_uint8((-(t2 + src[stride*9]) + 2*(t3 + src[stride*8]) + 6*t1 + 4)>>3);
        tmp[x]= t3;
        tmp2[x]= t1;
        src++;
    }
#endif 
}
