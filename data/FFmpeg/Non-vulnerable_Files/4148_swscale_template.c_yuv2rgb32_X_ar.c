static void RENAME(yuv2rgb32_X_ar)(SwsContext *c, const int16_t *lumFilter,
                                   const int16_t **lumSrc, int lumFilterSize,
                                   const int16_t *chrFilter, const int16_t **chrUSrc,
                                   const int16_t **chrVSrc,
                                   int chrFilterSize, const int16_t **alpSrc,
                                   uint8_t *dest, int dstW, int dstY)
{
    x86_reg dummy=0;
    x86_reg dstW_reg = dstW;
    x86_reg uv_off = c->uv_offx2;
    if (CONFIG_SWSCALE_ALPHA && c->alpPixBuf) {
        YSCALEYUV2PACKEDX_ACCURATE
        YSCALEYUV2RGBX
        "movq                      %%mm2, "U_TEMP"(%0)  \n\t"
        "movq                      %%mm4, "V_TEMP"(%0)  \n\t"
        "movq                      %%mm5, "Y_TEMP"(%0)  \n\t"
        YSCALEYUV2PACKEDX_ACCURATE_YA(ALP_MMX_FILTER_OFFSET)
        "movq               "Y_TEMP"(%0), %%mm5         \n\t"
        "psraw                        $3, %%mm1         \n\t"
        "psraw                        $3, %%mm7         \n\t"
        "packuswb                  %%mm7, %%mm1         \n\t"
        WRITEBGR32(%4, "%5", %%REGa, %%mm3, %%mm4, %%mm5, %%mm1, %%mm0, %%mm7, %%mm2, %%mm6)
        YSCALEYUV2PACKEDX_END
    } else {
        YSCALEYUV2PACKEDX_ACCURATE
        YSCALEYUV2RGBX
        "pcmpeqd %%mm7, %%mm7 \n\t"
        WRITEBGR32(%4, "%5", %%REGa, %%mm2, %%mm4, %%mm5, %%mm7, %%mm0, %%mm1, %%mm3, %%mm6)
        YSCALEYUV2PACKEDX_END
    }
}
