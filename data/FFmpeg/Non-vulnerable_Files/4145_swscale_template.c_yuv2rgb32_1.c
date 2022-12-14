static void RENAME(yuv2rgb32_1)(SwsContext *c, const int16_t *buf0,
                                const int16_t *ubuf[2], const int16_t *vbuf[2],
                                const int16_t *abuf0, uint8_t *dest,
                                int dstW, int uvalpha, int y)
{
    const int16_t *ubuf0 = ubuf[0];
    const int16_t *buf1= buf0; 
    if (uvalpha < 2048) { 
        const int16_t *ubuf1 = ubuf[0];
        if (CONFIG_SWSCALE_ALPHA && c->alpPixBuf) {
            __asm__ volatile(
                "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"
                "mov        %4, %%"REG_b"               \n\t"
                "push %%"REG_BP"                        \n\t"
                YSCALEYUV2RGB1(%%REGBP, %5)
                YSCALEYUV2RGB1_ALPHA(%%REGBP)
                WRITEBGR32(%%REGb, DSTW_OFFSET"(%5)", %%REGBP, %%mm2, %%mm4, %%mm5, %%mm7, %%mm0, %%mm1, %%mm3, %%mm6)
                "pop %%"REG_BP"                         \n\t"
                "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"
                :: "c" (buf0), "d" (abuf0), "S" (ubuf0), "D" (ubuf1), "m" (dest),
                   "a" (&c->redDither)
            );
        } else {
            __asm__ volatile(
                "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"
                "mov        %4, %%"REG_b"               \n\t"
                "push %%"REG_BP"                        \n\t"
                YSCALEYUV2RGB1(%%REGBP, %5)
                "pcmpeqd %%mm7, %%mm7                   \n\t"
                WRITEBGR32(%%REGb, DSTW_OFFSET"(%5)", %%REGBP, %%mm2, %%mm4, %%mm5, %%mm7, %%mm0, %%mm1, %%mm3, %%mm6)
                "pop %%"REG_BP"                         \n\t"
                "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"
                :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),
                   "a" (&c->redDither)
            );
        }
    } else {
        const int16_t *ubuf1 = ubuf[1];
        if (CONFIG_SWSCALE_ALPHA && c->alpPixBuf) {
            __asm__ volatile(
                "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"
                "mov        %4, %%"REG_b"               \n\t"
                "push %%"REG_BP"                        \n\t"
                YSCALEYUV2RGB1b(%%REGBP, %5)
                YSCALEYUV2RGB1_ALPHA(%%REGBP)
                WRITEBGR32(%%REGb, DSTW_OFFSET"(%5)", %%REGBP, %%mm2, %%mm4, %%mm5, %%mm7, %%mm0, %%mm1, %%mm3, %%mm6)
                "pop %%"REG_BP"                         \n\t"
                "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"
                :: "c" (buf0), "d" (abuf0), "S" (ubuf0), "D" (ubuf1), "m" (dest),
                   "a" (&c->redDither)
            );
        } else {
            __asm__ volatile(
                "mov %%"REG_b", "ESP_OFFSET"(%5)        \n\t"
                "mov        %4, %%"REG_b"               \n\t"
                "push %%"REG_BP"                        \n\t"
                YSCALEYUV2RGB1b(%%REGBP, %5)
                "pcmpeqd %%mm7, %%mm7                   \n\t"
                WRITEBGR32(%%REGb, DSTW_OFFSET"(%5)", %%REGBP, %%mm2, %%mm4, %%mm5, %%mm7, %%mm0, %%mm1, %%mm3, %%mm6)
                "pop %%"REG_BP"                         \n\t"
                "mov "ESP_OFFSET"(%5), %%"REG_b"        \n\t"
                :: "c" (buf0), "d" (buf1), "S" (ubuf0), "D" (ubuf1), "m" (dest),
                   "a" (&c->redDither)
            );
        }
    }
}
