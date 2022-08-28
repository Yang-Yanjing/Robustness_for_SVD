static inline void RENAME(deInterlaceMedian)(uint8_t src[], int stride)
{
#if TEMPLATE_PP_MMX
    src+= 4*stride;
#if TEMPLATE_PP_MMXEXT
    __asm__ volatile(
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_d"      \n\t"


        "movq (%0), %%mm0                       \n\t"
        "movq (%%"REG_a", %1), %%mm2            \n\t"
        "movq (%%"REG_a"), %%mm1                \n\t"
        "movq %%mm0, %%mm3                      \n\t"
        "pmaxub %%mm1, %%mm0                    \n\t"
        "pminub %%mm3, %%mm1                    \n\t"
        "pmaxub %%mm2, %%mm1                    \n\t"
        "pminub %%mm1, %%mm0                    \n\t"
        "movq %%mm0, (%%"REG_a")                \n\t"
        "movq (%0, %1, 4), %%mm0                \n\t"
        "movq (%%"REG_a", %1, 2), %%mm1         \n\t"
        "movq %%mm2, %%mm3                      \n\t"
        "pmaxub %%mm1, %%mm2                    \n\t"
        "pminub %%mm3, %%mm1                    \n\t"
        "pmaxub %%mm0, %%mm1                    \n\t"
        "pminub %%mm1, %%mm2                    \n\t"
        "movq %%mm2, (%%"REG_a", %1, 2)         \n\t"
        "movq (%%"REG_d"), %%mm2                \n\t"
        "movq (%%"REG_d", %1), %%mm1            \n\t"
        "movq %%mm2, %%mm3                      \n\t"
        "pmaxub %%mm0, %%mm2                    \n\t"
        "pminub %%mm3, %%mm0                    \n\t"
        "pmaxub %%mm1, %%mm0                    \n\t"
        "pminub %%mm0, %%mm2                    \n\t"
        "movq %%mm2, (%%"REG_d")                \n\t"
        "movq (%%"REG_d", %1, 2), %%mm2         \n\t"
        "movq (%0, %1, 8), %%mm0                \n\t"
        "movq %%mm2, %%mm3                      \n\t"
        "pmaxub %%mm0, %%mm2                    \n\t"
        "pminub %%mm3, %%mm0                    \n\t"
        "pmaxub %%mm1, %%mm0                    \n\t"
        "pminub %%mm0, %%mm2                    \n\t"
        "movq %%mm2, (%%"REG_d", %1, 2)         \n\t"
        : : "r" (src), "r" ((x86_reg)stride)
        : "%"REG_a, "%"REG_d
    );
#else 
    __asm__ volatile(
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_d"      \n\t"


        "pxor %%mm7, %%mm7                      \n\t"
#define REAL_MEDIAN(a,b,c)\
        "movq " #a ", %%mm0                     \n\t"\
        "movq " #b ", %%mm2                     \n\t"\
        "movq " #c ", %%mm1                     \n\t"\
        "movq %%mm0, %%mm3                      \n\t"\
        "movq %%mm1, %%mm4                      \n\t"\
        "movq %%mm2, %%mm5                      \n\t"\
        "psubusb %%mm1, %%mm3                   \n\t"\
        "psubusb %%mm2, %%mm4                   \n\t"\
        "psubusb %%mm0, %%mm5                   \n\t"\
        "pcmpeqb %%mm7, %%mm3                   \n\t"\
        "pcmpeqb %%mm7, %%mm4                   \n\t"\
        "pcmpeqb %%mm7, %%mm5                   \n\t"\
        "movq %%mm3, %%mm6                      \n\t"\
        "pxor %%mm4, %%mm3                      \n\t"\
        "pxor %%mm5, %%mm4                      \n\t"\
        "pxor %%mm6, %%mm5                      \n\t"\
        "por %%mm3, %%mm1                       \n\t"\
        "por %%mm4, %%mm2                       \n\t"\
        "por %%mm5, %%mm0                       \n\t"\
        "pand %%mm2, %%mm0                      \n\t"\
        "pand %%mm1, %%mm0                      \n\t"\
        "movq %%mm0, " #b "                     \n\t"
#define MEDIAN(a,b,c)  REAL_MEDIAN(a,b,c)
MEDIAN((%0)        , (%%REGa)       , (%%REGa, %1))
MEDIAN((%%REGa, %1), (%%REGa, %1, 2), (%0, %1, 4))
MEDIAN((%0, %1, 4) , (%%REGd)       , (%%REGd, %1))
MEDIAN((%%REGd, %1), (%%REGd, %1, 2), (%0, %1, 8))
        : : "r" (src), "r" ((x86_reg)stride)
        : "%"REG_a, "%"REG_d
    );
#endif 
#else 
    int x, y;
    src+= 4*stride;
    
    for(x=0; x<8; x++){
        uint8_t *colsrc = src;
        for (y=0; y<4; y++){
            int a, b, c, d, e, f;
            a = colsrc[0       ];
            b = colsrc[stride  ];
            c = colsrc[stride*2];
            d = (a-b)>>31;
            e = (b-c)>>31;
            f = (c-a)>>31;
            colsrc[stride  ] = (a|(d^f)) & (b|(d^e)) & (c|(e^f));
            colsrc += stride*2;
        }
        src++;
    }
#endif 
}
