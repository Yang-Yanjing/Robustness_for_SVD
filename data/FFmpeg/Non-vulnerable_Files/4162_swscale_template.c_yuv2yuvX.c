static void RENAME(yuv2yuvX)(const int16_t *filter, int filterSize,
                           const int16_t **src, uint8_t *dest, int dstW,
                           const uint8_t *dither, int offset)
{
    dither_8to16(dither, offset);
    filterSize--;
    __asm__ volatile(
        "movd %0, %%mm1\n\t"
        "punpcklwd %%mm1, %%mm1\n\t"
        "punpckldq %%mm1, %%mm1\n\t"
        "psllw        $3, %%mm1\n\t"
        "paddw     %%mm1, %%mm3\n\t"
        "paddw     %%mm1, %%mm4\n\t"
        "psraw        $4, %%mm3\n\t"
        "psraw        $4, %%mm4\n\t"
        ::"m"(filterSize)
     );
    __asm__ volatile(\
        "movq    %%mm3, %%mm6\n\t"
        "movq    %%mm4, %%mm7\n\t"
        "movl %3, %%ecx\n\t"
        "mov                                 %0, %%"REG_d"  \n\t"\
        "mov                        (%%"REG_d"), %%"REG_S"  \n\t"\
        ".p2align                             4             \n\t" \
        "1:                                                 \n\t"\
        "movq                      8(%%"REG_d"), %%mm0      \n\t" \
        "movq                (%%"REG_S", %%"REG_c", 2), %%mm2      \n\t" \
        "movq               8(%%"REG_S", %%"REG_c", 2), %%mm5      \n\t" \
        "add                                $16, %%"REG_d"  \n\t"\
        "mov                        (%%"REG_d"), %%"REG_S"  \n\t"\
        "test                         %%"REG_S", %%"REG_S"  \n\t"\
        "pmulhw                           %%mm0, %%mm2      \n\t"\
        "pmulhw                           %%mm0, %%mm5      \n\t"\
        "paddw                            %%mm2, %%mm3      \n\t"\
        "paddw                            %%mm5, %%mm4      \n\t"\
        " jnz                                1b             \n\t"\
        "psraw                               $3, %%mm3      \n\t"\
        "psraw                               $3, %%mm4      \n\t"\
        "packuswb                         %%mm4, %%mm3      \n\t"
        MOVNTQ2 "                         %%mm3, (%1, %%"REG_c")\n\t"
        "add                          $8, %%"REG_c"         \n\t"\
        "cmp                          %2, %%"REG_c"         \n\t"\
        "movq    %%mm6, %%mm3\n\t"
        "movq    %%mm7, %%mm4\n\t"
        "mov                                 %0, %%"REG_d"  \n\t"\
        "mov                        (%%"REG_d"), %%"REG_S"  \n\t"\
        "jb                                  1b             \n\t"\
        :: "g" (filter),
           "r" (dest-offset), "g" ((x86_reg)(dstW+offset)), "m" (offset)
        : "%"REG_d, "%"REG_S, "%"REG_c
    );
}
