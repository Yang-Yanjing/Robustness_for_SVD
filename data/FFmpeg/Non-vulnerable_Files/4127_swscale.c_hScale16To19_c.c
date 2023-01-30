static void hScale16To19_c(SwsContext *c, int16_t *_dst, int dstW,
                           const uint8_t *_src, const int16_t *filter,
                           const int32_t *filterPos, int filterSize)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(c->srcFormat);
    int i;
    int32_t *dst        = (int32_t *) _dst;
    const uint16_t *src = (const uint16_t *) _src;
    int bits            = desc->comp[0].depth_minus1;
    int sh              = bits - 4;
    if((isAnyRGB(c->srcFormat) || c->srcFormat==AV_PIX_FMT_PAL8) && desc->comp[0].depth_minus1<15)
        sh= 9;
    for (i = 0; i < dstW; i++) {
        int j;
        int srcPos = filterPos[i];
        int val    = 0;
        for (j = 0; j < filterSize; j++) {
            val += src[srcPos + j] * filter[filterSize * i + j];
        }
        
        dst[i] = FFMIN(val >> sh, (1 << 19) - 1);
    }
}
