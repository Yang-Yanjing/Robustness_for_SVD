static inline void RENAME(deInterlaceInterpolateLinear)(uint8_t src[], int stride)
{
#if TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW
    src+= 4*stride;
    __asm__ volatile(
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_c"      \n\t"


        "movq (%0), %%mm0                       \n\t"
        "movq (%%"REG_a", %1), %%mm1            \n\t"
        PAVGB(%%mm1, %%mm0)
        "movq %%mm0, (%%"REG_a")                \n\t"
        "movq (%0, %1, 4), %%mm0                \n\t"
        PAVGB(%%mm0, %%mm1)
        "movq %%mm1, (%%"REG_a", %1, 2)         \n\t"
        "movq (%%"REG_c", %1), %%mm1            \n\t"
        PAVGB(%%mm1, %%mm0)
        "movq %%mm0, (%%"REG_c")                \n\t"
        "movq (%0, %1, 8), %%mm0                \n\t"
        PAVGB(%%mm0, %%mm1)
        "movq %%mm1, (%%"REG_c", %1, 2)         \n\t"
        : : "r" (src), "r" ((x86_reg)stride)
        : "%"REG_a, "%"REG_c
    );
#else
    int a, b, x;
    src+= 4*stride;
    for(x=0; x<2; x++){
        a= *(uint32_t*)&src[stride*0];
        b= *(uint32_t*)&src[stride*2];
        *(uint32_t*)&src[stride*1]= (a|b) - (((a^b)&0xFEFEFEFEUL)>>1);
        a= *(uint32_t*)&src[stride*4];
        *(uint32_t*)&src[stride*3]= (a|b) - (((a^b)&0xFEFEFEFEUL)>>1);
        b= *(uint32_t*)&src[stride*6];
        *(uint32_t*)&src[stride*5]= (a|b) - (((a^b)&0xFEFEFEFEUL)>>1);
        a= *(uint32_t*)&src[stride*8];
        *(uint32_t*)&src[stride*7]= (a|b) - (((a^b)&0xFEFEFEFEUL)>>1);
        src += 4;
    }
#endif
}
