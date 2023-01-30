static int lum_h_scale(SwsContext *c, SwsFilterDescriptor *desc, int sliceY, int sliceH)
{
    FilterContext *instance = desc->instance;
    int srcW = desc->src->width;
    int dstW = desc->dst->width;
    int xInc = instance->xInc;
    int i;
    for (i = 0; i < sliceH; ++i) {
        uint8_t ** src = desc->src->plane[0].line;
        uint8_t ** dst = desc->dst->plane[0].line;
        int src_pos = sliceY+i - desc->src->plane[0].sliceY;
        int dst_pos = sliceY+i - desc->dst->plane[0].sliceY;
        if (c->hyscale_fast) {
            c->hyscale_fast(c, (int16_t*)dst[dst_pos], dstW, src[src_pos], srcW, xInc);
        } else {
            c->hyScale(c, (int16_t*)dst[dst_pos], dstW, (const uint8_t *)src[src_pos], instance->filter,
                       instance->filter_pos, instance->filter_size);
        }
        if (c->lumConvertRange)
            c->lumConvertRange((int16_t*)dst[dst_pos], dstW);
        desc->dst->plane[0].sliceH += 1;
        if (desc->alpha) {
            src = desc->src->plane[3].line;
            dst = desc->dst->plane[3].line;
            src_pos = sliceY+i - desc->src->plane[3].sliceY;
            dst_pos = sliceY+i - desc->dst->plane[3].sliceY;
            desc->dst->plane[3].sliceH += 1;
            if (c->hyscale_fast) {
                c->hyscale_fast(c, (int16_t*)dst[dst_pos], dstW, src[src_pos], srcW, xInc);
            } else {
                c->hyScale(c, (int16_t*)dst[dst_pos], dstW, (const uint8_t *)src[src_pos], instance->filter,
                            instance->filter_pos, instance->filter_size);
            }
        }
    }
    return sliceH;
}
