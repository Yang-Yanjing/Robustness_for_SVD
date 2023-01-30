static void RENAME(yuv2rgb32_2)(SwsContext *c, const int16_t *buf[2],
                                const int16_t *ubuf[2], const int16_t *vbuf[2],
                                const int16_t *abuf[2], uint8_t *dest,
                                int dstW, int yalpha, int uvalpha, int y)
{
    const int16_t *buf0  = buf[0],  *buf1  = buf[1],
                  *ubuf0 = ubuf[0], *ubuf1 = ubuf[1];
    if (CONFIG_SWSCALE_ALPHA && c->alpPixBuf) {
        const int16_t *abuf0 = abuf[0], *abuf1 = abuf[1];
#if ARCH_X86_64
        __asm__ volatile(
            YSCALEYUV2RGB(%%r8, %5)
            YSCALEYUV2RGB_YA(%%r8, %5, %6, %7)
            "psraw                  $3, %%mm1       \n\t" 
            "psraw                  $3, %%mm7       \n\t" 
            "packuswb            %%mm7, %%mm1       \n\t"
            WRITEBGR32(%4, DSTW_OFFSET"(%5)", %%r8, %%mm2, %%mm4, %%mm5, %%mm1, %%mm0, %%mm7, %%mm3, %%mm6)
            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "r" (dest),
               "a" (&c->redDither),
               "r" (abuf0), "r" (abuf1)
            : "%r8"
        );
#else
        c->u_temp=(intptr_t)abuf0;
        c->v_temp=(intptr_t)abuf1;
        __asm__ volatile(
            "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"
            "mov        %4, %%"REG_b"               \n\t"
            "push %%"REG_BP"                        \n\t"
            YSCALEYUV2RGB(%%REGBP, %5)
            "push                   %0              \n\t"
            "push                   %1              \n\t"
            "mov          "U_TEMP"(%5), %0          \n\t"
            "mov          "V_TEMP"(%5), %1          \n\t"
            YSCALEYUV2RGB_YA(%%REGBP, %5, %0, %1)
            "psraw                  $3, %%mm1       \n\t" 
            "psraw                  $3, %%mm7       \n\t" 
            "packuswb            %%mm7, %%mm1       \n\t"
            "pop                    %1              \n\t"
            "pop                    %0              \n\t"
            WRITEBGR32(%%REGb, DSTW_OFFSET"(%5)", %%REGBP, %%mm2, %%mm4, %%mm5, %%mm1, %%mm0, %%mm7, %%mm3, %%mm6)
            "pop %%"REG_BP"                         \n\t"
            "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"
            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),
               "a" (&c->redDither)
        );
#endif
    } else {
        __asm__ volatile(
            "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"
            "mov        %4, %%"REG_b"               \n\t"
            "push %%"REG_BP"                        \n\t"
            YSCALEYUV2RGB(%%REGBP, %5)
            "pcmpeqd %%mm7, %%mm7                   \n\t"
            WRITEBGR32(%%REGb, DSTW_OFFSET"(%5)", %%REGBP, %%mm2, %%mm4, %%mm5, %%mm7, %%mm0, %%mm1, %%mm3, %%mm6)
            "pop %%"REG_BP"                         \n\t"
            "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"
            :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),
               "a" (&c->redDither)
        );
    }
}
