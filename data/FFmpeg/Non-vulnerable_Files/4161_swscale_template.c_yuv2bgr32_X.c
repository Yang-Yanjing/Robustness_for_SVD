static void RENAME(yuv2bgr32_X)(SwsContext *c, const int16_t *lumFilter,
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
        YSCALEYUV2PACKEDX
        YSCALEYUV2RGBX
        YSCALEYUV2PACKEDX_YA(ALP_MMX_FILTER_OFFSET, %%mm0, %%mm3, %%mm6, %%mm1, %%mm7)
        "psraw                        $3, %%mm1         \n\t"
        "psraw                        $3, %%mm7         \n\t"
        "packuswb                  %%mm7, %%mm1         \n\t"
        WRITEBGR32(%4, "%5", %%REGa, %%mm5, %%mm4, %%mm2, %%mm1, %%mm0, %%mm7, %%mm3, %%mm6)
        YSCALEYUV2PACKEDX_END
    } else {
        YSCALEYUV2PACKEDX
        YSCALEYUV2RGBX
        "pcmpeqd %%mm7, %%mm7 \n\t"
        WRITEBGR32(%4, "%5", %%REGa, %%mm5, %%mm4, %%mm2, %%mm7, %%mm0, %%mm1, %%mm3, %%mm6)
        YSCALEYUV2PACKEDX_END
    }
}
