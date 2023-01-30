static void RENAME(yuv2rgb555_X_ar)(SwsContext *c, const int16_t *lumFilter,
                                    const int16_t **lumSrc, int lumFilterSize,
                                    const int16_t *chrFilter, const int16_t **chrUSrc,
                                    const int16_t **chrVSrc,
                                    int chrFilterSize, const int16_t **alpSrc,
                                    uint8_t *dest, int dstW, int dstY)
{
    x86_reg dummy=0;
    x86_reg dstW_reg = dstW;
    x86_reg uv_off = c->uv_offx2;
    YSCALEYUV2PACKEDX_ACCURATE
    YSCALEYUV2RGBX
    "pxor %%mm7, %%mm7 \n\t"
    
#ifdef DITHER1XBPP
    "paddusb "BLUE_DITHER"(%0), %%mm2\n\t"
    "paddusb "GREEN_DITHER"(%0), %%mm4\n\t"
    "paddusb "RED_DITHER"(%0), %%mm5\n\t"
#endif
    WRITERGB15(%4, "%5", %%REGa)
    YSCALEYUV2PACKEDX_END
}
