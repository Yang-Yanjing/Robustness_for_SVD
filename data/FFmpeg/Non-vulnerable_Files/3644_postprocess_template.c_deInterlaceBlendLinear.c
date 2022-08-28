static inline void RENAME(deInterlaceBlendLinear)(uint8_t src[], int stride, uint8_t *tmp)
{
#if TEMPLATE_PP_MMXEXT || TEMPLATE_PP_3DNOW
    src+= 4*stride;
    __asm__ volatile(
        "lea (%0, %1), %%"REG_a"                \n\t"
        "lea (%%"REG_a", %1, 4), %%"REG_d"      \n\t"


        "movq (%2), %%mm0                       \n\t" 
        "movq (%%"REG_a"), %%mm1                \n\t" 
        PAVGB(%%mm1, %%mm0)                           
        "movq (%0), %%mm2                       \n\t" 
        PAVGB(%%mm2, %%mm0)
        "movq %%mm0, (%0)                       \n\t"
        "movq (%%"REG_a", %1), %%mm0            \n\t" 
        PAVGB(%%mm0, %%mm2)                           
        PAVGB(%%mm1, %%mm2)                           
        "movq %%mm2, (%%"REG_a")                \n\t"
        "movq (%%"REG_a", %1, 2), %%mm2         \n\t" 
        PAVGB(%%mm2, %%mm1)                           
        PAVGB(%%mm0, %%mm1)                           
        "movq %%mm1, (%%"REG_a", %1)            \n\t"
        "movq (%0, %1, 4), %%mm1                \n\t" 
        PAVGB(%%mm1, %%mm0)                           
        PAVGB(%%mm2, %%mm0)                           
        "movq %%mm0, (%%"REG_a", %1, 2)         \n\t"
        "movq (%%"REG_d"), %%mm0                \n\t" 
        PAVGB(%%mm0, %%mm2)                           
        PAVGB(%%mm1, %%mm2)                           
        "movq %%mm2, (%0, %1, 4)                \n\t"
        "movq (%%"REG_d", %1), %%mm2            \n\t" 
        PAVGB(%%mm2, %%mm1)                           
        PAVGB(%%mm0, %%mm1)                           
        "movq %%mm1, (%%"REG_d")                \n\t"
        "movq (%%"REG_d", %1, 2), %%mm1         \n\t" 
        PAVGB(%%mm1, %%mm0)                           
        PAVGB(%%mm2, %%mm0)                           
        "movq %%mm0, (%%"REG_d", %1)            \n\t"
        "movq (%0, %1, 8), %%mm0                \n\t" 
        PAVGB(%%mm0, %%mm2)                           
        PAVGB(%%mm1, %%mm2)                           
        "movq %%mm2, (%%"REG_d", %1, 2)         \n\t"
        "movq %%mm1, (%2)                       \n\t"
        : : "r" (src), "r" ((x86_reg)stride), "r" (tmp)
        : "%"REG_a, "%"REG_d
    );
#else 
    int a, b, c, x;
    src+= 4*stride;
    for(x=0; x<2; x++){
        a= *(uint32_t*)&tmp[stride*0];
        b= *(uint32_t*)&src[stride*0];
        c= *(uint32_t*)&src[stride*1];
        a= (a&c) + (((a^c)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*0]= (a|b) - (((a^b)&0xFEFEFEFEUL)>>1);
        a= *(uint32_t*)&src[stride*2];
        b= (a&b) + (((a^b)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*1]= (c|b) - (((c^b)&0xFEFEFEFEUL)>>1);
        b= *(uint32_t*)&src[stride*3];
        c= (b&c) + (((b^c)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*2]= (c|a) - (((c^a)&0xFEFEFEFEUL)>>1);
        c= *(uint32_t*)&src[stride*4];
        a= (a&c) + (((a^c)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*3]= (a|b) - (((a^b)&0xFEFEFEFEUL)>>1);
        a= *(uint32_t*)&src[stride*5];
        b= (a&b) + (((a^b)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*4]= (c|b) - (((c^b)&0xFEFEFEFEUL)>>1);
        b= *(uint32_t*)&src[stride*6];
        c= (b&c) + (((b^c)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*5]= (c|a) - (((c^a)&0xFEFEFEFEUL)>>1);
        c= *(uint32_t*)&src[stride*7];
        a= (a&c) + (((a^c)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*6]= (a|b) - (((a^b)&0xFEFEFEFEUL)>>1);
        a= *(uint32_t*)&src[stride*8];
        b= (a&b) + (((a^b)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&src[stride*7]= (c|b) - (((c^b)&0xFEFEFEFEUL)>>1);
        *(uint32_t*)&tmp[stride*0]= c;
        src += 4;
        tmp += 4;
    }
#endif 
}
