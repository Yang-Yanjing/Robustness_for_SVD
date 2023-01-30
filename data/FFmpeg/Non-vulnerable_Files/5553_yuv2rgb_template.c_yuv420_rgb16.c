static inline int RENAME(yuv420_rgb16)(SwsContext *c, const uint8_t *src[],
                                       int srcStride[],
                                       int srcSliceY, int srcSliceH,
                                       uint8_t *dst[], int dstStride[])
{
    int y, h_size, vshift;
    YUV2RGB_LOOP(2)
#ifdef DITHER1XBPP
        c->blueDither  = ff_dither8[y       & 1];
        c->greenDither = ff_dither4[y       & 1];
        c->redDither   = ff_dither8[(y + 1) & 1];
#endif
        YUV2RGB_INITIAL_LOAD
        YUV2RGB
        RGB_PACK_INTERLEAVE
#ifdef DITHER1XBPP
        DITHER_RGB
#endif
        RGB_PACK16(pb_07, 0)
    YUV2RGB_ENDLOOP(2)
    YUV2RGB_OPERANDS
    YUV2RGB_ENDFUNC
}
