static inline void RENAME(deInterlaceInterpolateCubic)(uint8_t src[], int stride)
{
#if TEMPLATE_PP_SSE2 || TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW
    src+= stride*3;
    __asm__ volatile(
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_d"      \n\t"
        "lea (%%"REG_d", %1, 4), %%"REG_c"      \n\t"
        "add %1, %%"REG_c"                      \n\t"
#if TEMPLATE_PP_SSE2
        "pxor %%xmm7, %%xmm7                    \n\t"
#define REAL_DEINT_CUBIC(a,b,c,d,e)\
        "movq " #a ", %%xmm0                    \n\t"\
        "movq " #b ", %%xmm1                    \n\t"\
        "movq " #d ", %%xmm2                    \n\t"\
        "movq " #e ", %%xmm3                    \n\t"\
        "pavgb %%xmm2, %%xmm1                   \n\t"\
        "pavgb %%xmm3, %%xmm0                   \n\t"\
        "punpcklbw %%xmm7, %%xmm0               \n\t"\
        "punpcklbw %%xmm7, %%xmm1               \n\t"\
        "psubw %%xmm1, %%xmm0                   \n\t"\
        "psraw $3, %%xmm0                       \n\t"\
        "psubw %%xmm0, %%xmm1                   \n\t"\
        "packuswb %%xmm1, %%xmm1                \n\t"\
        "movlps %%xmm1, " #c "                  \n\t"
#else 
        "pxor %%mm7, %%mm7                      \n\t"


#define REAL_DEINT_CUBIC(a,b,c,d,e)\
        "movq " #a ", %%mm0                     \n\t"\
        "movq " #b ", %%mm1                     \n\t"\
        "movq " #d ", %%mm2                     \n\t"\
        "movq " #e ", %%mm3                     \n\t"\
        PAVGB(%%mm2, %%mm1)                             \
        PAVGB(%%mm3, %%mm0)                             \
        "movq %%mm0, %%mm2                      \n\t"\
        "punpcklbw %%mm7, %%mm0                 \n\t"\
        "punpckhbw %%mm7, %%mm2                 \n\t"\
        "movq %%mm1, %%mm3                      \n\t"\
        "punpcklbw %%mm7, %%mm1                 \n\t"\
        "punpckhbw %%mm7, %%mm3                 \n\t"\
        "psubw %%mm1, %%mm0                     \n\t"   \
        "psubw %%mm3, %%mm2                     \n\t"   \
        "psraw $3, %%mm0                        \n\t"   \
        "psraw $3, %%mm2                        \n\t"   \
        "psubw %%mm0, %%mm1                     \n\t"   \
        "psubw %%mm2, %%mm3                     \n\t"   \
        "packuswb %%mm3, %%mm1                  \n\t"\
        "movq %%mm1, " #c "                     \n\t"
#endif 
#define DEINT_CUBIC(a,b,c,d,e)  REAL_DEINT_CUBIC(a,b,c,d,e)
DEINT_CUBIC((%0)        , (%%REGa, %1), (%%REGa, %1, 2), (%0, %1, 4) , (%%REGd, %1))
DEINT_CUBIC((%%REGa, %1), (%0, %1, 4) , (%%REGd)       , (%%REGd, %1), (%0, %1, 8))
DEINT_CUBIC((%0, %1, 4) , (%%REGd, %1), (%%REGd, %1, 2), (%0, %1, 8) , (%%REGc))
DEINT_CUBIC((%%REGd, %1), (%0, %1, 8) , (%%REGd, %1, 4), (%%REGc)    , (%%REGc, %1, 2))
        : : "r" (src), "r" ((x86_reg)stride)
        :
#if TEMPLATE_PP_SSE2
        XMM_CLOBBERS("%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm7",)
#endif
        "%"REG_a, "%"REG_d, "%"REG_c
    );
#undef REAL_DEINT_CUBIC
#else 
    int x;
    src+= stride*3;
    for(x=0; x<8; x++){
        src[stride*3] = av_clip_uint8((-src[0]        + 9*src[stride*2] + 9*src[stride*4] - src[stride*6])>>4);
        src[stride*5] = av_clip_uint8((-src[stride*2] + 9*src[stride*4] + 9*src[stride*6] - src[stride*8])>>4);
        src[stride*7] = av_clip_uint8((-src[stride*4] + 9*src[stride*6] + 9*src[stride*8] - src[stride*10])>>4);
        src[stride*9] = av_clip_uint8((-src[stride*6] + 9*src[stride*8] + 9*src[stride*10] - src[stride*12])>>4);
        src++;
    }
#endif 
}
