static inline void RENAME(tempNoiseReducer)(uint8_t *src, int stride,
                                    uint8_t *tempBlurred, uint32_t *tempBlurredPast, const int *maxNoise)
{
    
    tempBlurredPast[127]= maxNoise[0];
    tempBlurredPast[128]= maxNoise[1];
    tempBlurredPast[129]= maxNoise[2];
#define FAST_L2_DIFF

#if (TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW) && HAVE_6REGS
    __asm__ volatile(
        "lea (%2, %2, 2), %%"REG_a"             \n\t" 
        "lea (%2, %2, 4), %%"REG_d"             \n\t" 
        "lea (%%"REG_d", %2, 2), %%"REG_c"      \n\t" 



#ifdef L1_DIFF 
        "movq (%0), %%mm0                       \n\t" 
        "psadbw (%1), %%mm0                     \n\t" 
        "movq (%0, %2), %%mm1                   \n\t" 
        "psadbw (%1, %2), %%mm1                 \n\t" 
        "movq (%0, %2, 2), %%mm2                \n\t" 
        "psadbw (%1, %2, 2), %%mm2              \n\t" 
        "movq (%0, %%"REG_a"), %%mm3            \n\t" 
        "psadbw (%1, %%"REG_a"), %%mm3          \n\t" 
        "movq (%0, %2, 4), %%mm4                \n\t" 
        "paddw %%mm1, %%mm0                     \n\t"
        "psadbw (%1, %2, 4), %%mm4              \n\t" 
        "movq (%0, %%"REG_d"), %%mm5            \n\t" 
        "paddw %%mm2, %%mm0                     \n\t"
        "psadbw (%1, %%"REG_d"), %%mm5          \n\t" 
        "movq (%0, %%"REG_a", 2), %%mm6         \n\t" 
        "paddw %%mm3, %%mm0                     \n\t"
        "psadbw (%1, %%"REG_a", 2), %%mm6       \n\t" 
        "movq (%0, %%"REG_c"), %%mm7            \n\t" 
        "paddw %%mm4, %%mm0                     \n\t"
        "psadbw (%1, %%"REG_c"), %%mm7          \n\t" 
        "paddw %%mm5, %%mm6                     \n\t"
        "paddw %%mm7, %%mm6                     \n\t"
        "paddw %%mm6, %%mm0                     \n\t"
#else 
#if defined (FAST_L2_DIFF)
        "pcmpeqb %%mm7, %%mm7                   \n\t"
        "movq "MANGLE(b80)", %%mm6              \n\t"
        "pxor %%mm0, %%mm0                      \n\t"
#define REAL_L2_DIFF_CORE(a, b)\
        "movq " #a ", %%mm5                     \n\t"\
        "movq " #b ", %%mm2                     \n\t"\
        "pxor %%mm7, %%mm2                      \n\t"\
        PAVGB(%%mm2, %%mm5)\
        "paddb %%mm6, %%mm5                     \n\t"\
        "movq %%mm5, %%mm2                      \n\t"\
        "psllw $8, %%mm5                        \n\t"\
        "pmaddwd %%mm5, %%mm5                   \n\t"\
        "pmaddwd %%mm2, %%mm2                   \n\t"\
        "paddd %%mm2, %%mm5                     \n\t"\
        "psrld $14, %%mm5                       \n\t"\
        "paddd %%mm5, %%mm0                     \n\t"
#else 
        "pxor %%mm7, %%mm7                      \n\t"
        "pxor %%mm0, %%mm0                      \n\t"
#define REAL_L2_DIFF_CORE(a, b)\
        "movq " #a ", %%mm5                     \n\t"\
        "movq " #b ", %%mm2                     \n\t"\
        "movq %%mm5, %%mm1                      \n\t"\
        "movq %%mm2, %%mm3                      \n\t"\
        "punpcklbw %%mm7, %%mm5                 \n\t"\
        "punpckhbw %%mm7, %%mm1                 \n\t"\
        "punpcklbw %%mm7, %%mm2                 \n\t"\
        "punpckhbw %%mm7, %%mm3                 \n\t"\
        "psubw %%mm2, %%mm5                     \n\t"\
        "psubw %%mm3, %%mm1                     \n\t"\
        "pmaddwd %%mm5, %%mm5                   \n\t"\
        "pmaddwd %%mm1, %%mm1                   \n\t"\
        "paddd %%mm1, %%mm5                     \n\t"\
        "paddd %%mm5, %%mm0                     \n\t"
#endif 
#define L2_DIFF_CORE(a, b)  REAL_L2_DIFF_CORE(a, b)
L2_DIFF_CORE((%0)          , (%1))
L2_DIFF_CORE((%0, %2)      , (%1, %2))
L2_DIFF_CORE((%0, %2, 2)   , (%1, %2, 2))
L2_DIFF_CORE((%0, %%REGa)  , (%1, %%REGa))
L2_DIFF_CORE((%0, %2, 4)   , (%1, %2, 4))
L2_DIFF_CORE((%0, %%REGd)  , (%1, %%REGd))
L2_DIFF_CORE((%0, %%REGa,2), (%1, %%REGa,2))
L2_DIFF_CORE((%0, %%REGc)  , (%1, %%REGc))
#endif 
        "movq %%mm0, %%mm4                      \n\t"
        "psrlq $32, %%mm0                       \n\t"
        "paddd %%mm0, %%mm4                     \n\t"
        "movd %%mm4, %%ecx                      \n\t"
        "shll $2, %%ecx                         \n\t"
        "mov %3, %%"REG_d"                      \n\t"
        "addl -4(%%"REG_d"), %%ecx              \n\t"
        "addl 4(%%"REG_d"), %%ecx               \n\t"
        "addl -1024(%%"REG_d"), %%ecx           \n\t"
        "addl $4, %%ecx                         \n\t"
        "addl 1024(%%"REG_d"), %%ecx            \n\t"
        "shrl $3, %%ecx                         \n\t"
        "movl %%ecx, (%%"REG_d")                \n\t"



        "cmpl 512(%%"REG_d"), %%ecx             \n\t"
        " jb 2f                                 \n\t"
        "cmpl 516(%%"REG_d"), %%ecx             \n\t"
        " jb 1f                                 \n\t"
        "lea (%%"REG_a", %2, 2), %%"REG_d"      \n\t" 
        "lea (%%"REG_d", %2, 2), %%"REG_c"      \n\t" 
        "movq (%0), %%mm0                       \n\t" 
        "movq (%0, %2), %%mm1                   \n\t" 
        "movq (%0, %2, 2), %%mm2                \n\t" 
        "movq (%0, %%"REG_a"), %%mm3            \n\t" 
        "movq (%0, %2, 4), %%mm4                \n\t" 
        "movq (%0, %%"REG_d"), %%mm5            \n\t" 
        "movq (%0, %%"REG_a", 2), %%mm6         \n\t" 
        "movq (%0, %%"REG_c"), %%mm7            \n\t" 
        "movq %%mm0, (%1)                       \n\t" 
        "movq %%mm1, (%1, %2)                   \n\t" 
        "movq %%mm2, (%1, %2, 2)                \n\t" 
        "movq %%mm3, (%1, %%"REG_a")            \n\t" 
        "movq %%mm4, (%1, %2, 4)                \n\t" 
        "movq %%mm5, (%1, %%"REG_d")            \n\t" 
        "movq %%mm6, (%1, %%"REG_a", 2)         \n\t" 
        "movq %%mm7, (%1, %%"REG_c")            \n\t" 
        "jmp 4f                                 \n\t"
        "1:                                     \n\t"
        "lea (%%"REG_a", %2, 2), %%"REG_d"      \n\t" 
        "lea (%%"REG_d", %2, 2), %%"REG_c"      \n\t" 
        "movq (%0), %%mm0                       \n\t" 
        PAVGB((%1), %%mm0)                            
        "movq (%0, %2), %%mm1                   \n\t" 
        PAVGB((%1, %2), %%mm1)                        
        "movq (%0, %2, 2), %%mm2                \n\t" 
        PAVGB((%1, %2, 2), %%mm2)                     
        "movq (%0, %%"REG_a"), %%mm3            \n\t" 
        PAVGB((%1, %%REGa), %%mm3)                    
        "movq (%0, %2, 4), %%mm4                \n\t" 
        PAVGB((%1, %2, 4), %%mm4)                     
        "movq (%0, %%"REG_d"), %%mm5            \n\t" 
        PAVGB((%1, %%REGd), %%mm5)                    
        "movq (%0, %%"REG_a", 2), %%mm6         \n\t" 
        PAVGB((%1, %%REGa, 2), %%mm6)                 
        "movq (%0, %%"REG_c"), %%mm7            \n\t" 
        PAVGB((%1, %%REGc), %%mm7)                    
        "movq %%mm0, (%1)                       \n\t" 
        "movq %%mm1, (%1, %2)                   \n\t" 
        "movq %%mm2, (%1, %2, 2)                \n\t" 
        "movq %%mm3, (%1, %%"REG_a")            \n\t" 
        "movq %%mm4, (%1, %2, 4)                \n\t" 
        "movq %%mm5, (%1, %%"REG_d")            \n\t" 
        "movq %%mm6, (%1, %%"REG_a", 2)         \n\t" 
        "movq %%mm7, (%1, %%"REG_c")            \n\t" 
        "movq %%mm0, (%0)                       \n\t" 
        "movq %%mm1, (%0, %2)                   \n\t" 
        "movq %%mm2, (%0, %2, 2)                \n\t" 
        "movq %%mm3, (%0, %%"REG_a")            \n\t" 
        "movq %%mm4, (%0, %2, 4)                \n\t" 
        "movq %%mm5, (%0, %%"REG_d")            \n\t" 
        "movq %%mm6, (%0, %%"REG_a", 2)         \n\t" 
        "movq %%mm7, (%0, %%"REG_c")            \n\t" 
        "jmp 4f                                 \n\t"
        "2:                                     \n\t"
        "cmpl 508(%%"REG_d"), %%ecx             \n\t"
        " jb 3f                                 \n\t"
        "lea (%%"REG_a", %2, 2), %%"REG_d"      \n\t" 
        "lea (%%"REG_d", %2, 2), %%"REG_c"      \n\t" 
        "movq (%0), %%mm0                       \n\t" 
        "movq (%0, %2), %%mm1                   \n\t" 
        "movq (%0, %2, 2), %%mm2                \n\t" 
        "movq (%0, %%"REG_a"), %%mm3            \n\t" 
        "movq (%1), %%mm4                       \n\t" 
        "movq (%1, %2), %%mm5                   \n\t" 
        "movq (%1, %2, 2), %%mm6                \n\t" 
        "movq (%1, %%"REG_a"), %%mm7            \n\t" 
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        "movq %%mm0, (%1)                       \n\t" 
        "movq %%mm1, (%1, %2)                   \n\t" 
        "movq %%mm2, (%1, %2, 2)                \n\t" 
        "movq %%mm3, (%1, %%"REG_a")            \n\t" 
        "movq %%mm0, (%0)                       \n\t" 
        "movq %%mm1, (%0, %2)                   \n\t" 
        "movq %%mm2, (%0, %2, 2)                \n\t" 
        "movq %%mm3, (%0, %%"REG_a")            \n\t" 
        "movq (%0, %2, 4), %%mm0                \n\t" 
        "movq (%0, %%"REG_d"), %%mm1            \n\t" 
        "movq (%0, %%"REG_a", 2), %%mm2         \n\t" 
        "movq (%0, %%"REG_c"), %%mm3            \n\t" 
        "movq (%1, %2, 4), %%mm4                \n\t" 
        "movq (%1, %%"REG_d"), %%mm5            \n\t" 
        "movq (%1, %%"REG_a", 2), %%mm6         \n\t" 
        "movq (%1, %%"REG_c"), %%mm7            \n\t" 
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        "movq %%mm0, (%1, %2, 4)                \n\t" 
        "movq %%mm1, (%1, %%"REG_d")            \n\t" 
        "movq %%mm2, (%1, %%"REG_a", 2)         \n\t" 
        "movq %%mm3, (%1, %%"REG_c")            \n\t" 
        "movq %%mm0, (%0, %2, 4)                \n\t" 
        "movq %%mm1, (%0, %%"REG_d")            \n\t" 
        "movq %%mm2, (%0, %%"REG_a", 2)         \n\t" 
        "movq %%mm3, (%0, %%"REG_c")            \n\t" 
        "jmp 4f                                 \n\t"
        "3:                                     \n\t"
        "lea (%%"REG_a", %2, 2), %%"REG_d"      \n\t" 
        "lea (%%"REG_d", %2, 2), %%"REG_c"      \n\t" 
        "movq (%0), %%mm0                       \n\t" 
        "movq (%0, %2), %%mm1                   \n\t" 
        "movq (%0, %2, 2), %%mm2                \n\t" 
        "movq (%0, %%"REG_a"), %%mm3            \n\t" 
        "movq (%1), %%mm4                       \n\t" 
        "movq (%1, %2), %%mm5                   \n\t" 
        "movq (%1, %2, 2), %%mm6                \n\t" 
        "movq (%1, %%"REG_a"), %%mm7            \n\t" 
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        "movq %%mm0, (%1)                       \n\t" 
        "movq %%mm1, (%1, %2)                   \n\t" 
        "movq %%mm2, (%1, %2, 2)                \n\t" 
        "movq %%mm3, (%1, %%"REG_a")            \n\t" 
        "movq %%mm0, (%0)                       \n\t" 
        "movq %%mm1, (%0, %2)                   \n\t" 
        "movq %%mm2, (%0, %2, 2)                \n\t" 
        "movq %%mm3, (%0, %%"REG_a")            \n\t" 
        "movq (%0, %2, 4), %%mm0                \n\t" 
        "movq (%0, %%"REG_d"), %%mm1            \n\t" 
        "movq (%0, %%"REG_a", 2), %%mm2         \n\t" 
        "movq (%0, %%"REG_c"), %%mm3            \n\t" 
        "movq (%1, %2, 4), %%mm4                \n\t" 
        "movq (%1, %%"REG_d"), %%mm5            \n\t" 
        "movq (%1, %%"REG_a", 2), %%mm6         \n\t" 
        "movq (%1, %%"REG_c"), %%mm7            \n\t" 
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        PAVGB(%%mm4, %%mm0)
        PAVGB(%%mm5, %%mm1)
        PAVGB(%%mm6, %%mm2)
        PAVGB(%%mm7, %%mm3)
        "movq %%mm0, (%1, %2, 4)                \n\t" 
        "movq %%mm1, (%1, %%"REG_d")            \n\t" 
        "movq %%mm2, (%1, %%"REG_a", 2)         \n\t" 
        "movq %%mm3, (%1, %%"REG_c")            \n\t" 
        "movq %%mm0, (%0, %2, 4)                \n\t" 
        "movq %%mm1, (%0, %%"REG_d")            \n\t" 
        "movq %%mm2, (%0, %%"REG_a", 2)         \n\t" 
        "movq %%mm3, (%0, %%"REG_c")            \n\t" 
        "4:                                     \n\t"
        :: "r" (src), "r" (tempBlurred), "r"((x86_reg)stride), "m" (tempBlurredPast)
          NAMED_CONSTRAINTS_ADD(b80)
        : "%"REG_a, "%"REG_d, "%"REG_c, "memory"
    );
#else 
{
    int y;
    int d=0;

    int i;
    for(y=0; y<8; y++){
        int x;
        for(x=0; x<8; x++){
            int ref= tempBlurred[ x + y*stride ];
            int cur= src[ x + y*stride ];
            int d1=ref - cur;



            d+= d1*d1;

        }
    }
    i=d;
    d=  (
        4*d
        +(*(tempBlurredPast-256))
        +(*(tempBlurredPast-1))+ (*(tempBlurredPast+1))
        +(*(tempBlurredPast+256))
        +4)>>3;
    *tempBlurredPast=i;








    if(d > maxNoise[1]){
        if(d < maxNoise[2]){
            for(y=0; y<8; y++){
                int x;
                for(x=0; x<8; x++){
                    int ref= tempBlurred[ x + y*stride ];
                    int cur= src[ x + y*stride ];
                    tempBlurred[ x + y*stride ]=
                    src[ x + y*stride ]=
                        (ref + cur + 1)>>1;
                }
            }
        }else{
            for(y=0; y<8; y++){
                int x;
                for(x=0; x<8; x++){
                    tempBlurred[ x + y*stride ]= src[ x + y*stride ];
                }
            }
        }
    }else{
        if(d < maxNoise[0]){
            for(y=0; y<8; y++){
                int x;
                for(x=0; x<8; x++){
                    int ref= tempBlurred[ x + y*stride ];
                    int cur= src[ x + y*stride ];
                    tempBlurred[ x + y*stride ]=
                    src[ x + y*stride ]=
                        (ref*7 + cur + 4)>>3;
                }
            }
        }else{
            for(y=0; y<8; y++){
                int x;
                for(x=0; x<8; x++){
                    int ref= tempBlurred[ x + y*stride ];
                    int cur= src[ x + y*stride ];
                    tempBlurred[ x + y*stride ]=
                    src[ x + y*stride ]=
                        (ref*3 + cur + 2)>>2;
                }
            }
        }
    }
}
#endif 

