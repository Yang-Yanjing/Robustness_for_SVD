static inline int RENAME(yuva420_rgb32)(SwsContext *c, const uint8_t *src[],
                                        int srcStride[],
                                        int srcSliceY, int srcSliceH,
                                        uint8_t *dst[], int dstStride[])
{
    int y, h_size, vshift;
    YUV2RGB_LOOP(4)
        const uint8_t *pa = src[3] + y * srcStride[3];
        YUV2RGB_INITIAL_LOAD
        YUV2RGB
        RGB_PACK_INTERLEAVE
        LOAD_ALPHA
        RGB_PACK32(REG_RED, REG_GREEN, REG_BLUE, REG_ALPHA)
    YUV2RGB_ENDLOOP(4)
    YUV2RGB_OPERANDS_ALPHA
    YUV2RGB_ENDFUNC
}
