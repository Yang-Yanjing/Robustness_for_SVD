static inline int RENAME(yuv420_rgb24)(SwsContext *c, const uint8_t *src[],
                                       int srcStride[],
                                       int srcSliceY, int srcSliceH,
                                       uint8_t *dst[], int dstStride[])
{
    int y, h_size, vshift;
    YUV2RGB_LOOP(3)
        YUV2RGB_INITIAL_LOAD
        YUV2RGB
        RGB_PACK24(REG_BLUE, REG_RED)
    YUV2RGB_ENDLOOP(3)
    YUV2RGB_OPERANDS
    YUV2RGB_ENDFUNC
}
