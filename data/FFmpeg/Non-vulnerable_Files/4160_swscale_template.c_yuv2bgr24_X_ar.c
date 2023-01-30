static void RENAME(yuv2bgr24_X_ar)(SwsContext *c, const int16_t *lumFilter,
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
    "lea (%%"REG_a", %%"REG_a", 2), %%"REG_c"\n\t" 
    "add %4, %%"REG_c"                        \n\t"
    WRITEBGR24(%%REGc, "%5", %%REGa)
    :: "r" (&c->redDither),
       "m" (dummy), "m" (dummy), "m" (dummy),
       "r" (dest), "m" (dstW_reg), "m"(uv_off)
       NAMED_CONSTRAINTS_ADD(ff_M24A,ff_M24C,ff_M24B)
    : "%"REG_a, "%"REG_c, "%"REG_d, "%"REG_S
    );
}
