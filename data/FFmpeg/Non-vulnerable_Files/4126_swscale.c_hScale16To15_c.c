static void hScale16To15_c(SwsContext *c, int16_t *dst, int dstW,
                           const uint8_t *_src, const int16_t *filter,
                           const int32_t *filterPos, int filterSize)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(c->srcFormat);
    int i;
    const uint16_t *src = (const uint16_t *) _src;
    int sh              = desc->comp[0].depth_minus1;
    if(sh<15)
        sh= isAnyRGB(c->srcFormat) || c->srcFormat==AV_PIX_FMT_PAL8 ? 13 : desc->comp[0].depth_minus1;
    for (i = 0; i < dstW; i++) {
        int j;
        int srcPos = filterPos[i];
        int val    = 0;
        for (j = 0; j < filterSize; j++) {
            val += src[srcPos + j] * filter[filterSize * i + j];
        }
        
        dst[i] = FFMIN(val >> sh, (1 << 15) - 1);
    }
}
