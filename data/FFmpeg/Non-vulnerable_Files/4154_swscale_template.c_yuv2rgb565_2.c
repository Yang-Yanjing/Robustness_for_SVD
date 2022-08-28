static void RENAME(yuv2rgb565_2)(SwsContext *c, const int16_t *buf[2],
                                 const int16_t *ubuf[2], const int16_t *vbuf[2],
                                 const int16_t *abuf[2], uint8_t *dest,
                                 int dstW, int yalpha, int uvalpha, int y)
{
    const int16_t *buf0  = buf[0],  *buf1  = buf[1],
                  *ubuf0 = ubuf[0], *ubuf1 = ubuf[1];
    __asm__ volatile(
        "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"
        "mov        %4, %%"REG_b"               \n\t"
        "push %%"REG_BP"                        \n\t"
        YSCALEYUV2RGB(%%REGBP, %5)
        "pxor    %%mm7, %%mm7                   \n\t"
        
#ifdef DITHER1XBPP
        "paddusb "BLUE_DITHER"(%5), %%mm2      \n\t"
        "paddusb "GREEN_DITHER"(%5), %%mm4      \n\t"
        "paddusb "RED_DITHER"(%5), %%mm5      \n\t"
#endif
        WRITERGB16(%%REGb, DSTW_OFFSET"(%5)", %%REGBP)
        "pop %%"REG_BP"                         \n\t"
        "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"
        :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),
           "a" (&c->redDither)
           NAMED_CONSTRAINTS_ADD(bF8,bFC)
    );
}
