static void RENAME(yuv2yuyv422_X)(SwsContext *c, const int16_t *lumFilter,
                                  const int16_t **lumSrc, int lumFilterSize,
                                  const int16_t *chrFilter, const int16_t **chrUSrc,
                                  const int16_t **chrVSrc,
                                  int chrFilterSize, const int16_t **alpSrc,
                                  uint8_t *dest, int dstW, int dstY)
{
    x86_reg dummy=0;
    x86_reg dstW_reg = dstW;
    x86_reg uv_off = c->uv_offx2;
    YSCALEYUV2PACKEDX
    
    "psraw $3, %%mm3    \n\t"
    "psraw $3, %%mm4    \n\t"
    "psraw $3, %%mm1    \n\t"
    "psraw $3, %%mm7    \n\t"
    WRITEYUY2(%4, "%5", %%REGa)
    YSCALEYUV2PACKEDX_END
}
