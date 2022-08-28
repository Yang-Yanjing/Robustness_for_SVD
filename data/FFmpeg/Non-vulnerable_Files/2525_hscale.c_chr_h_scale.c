static int chr_h_scale(SwsContext *c, SwsFilterDescriptor *desc, int sliceY, int sliceH)
{
    FilterContext *instance = desc->instance;
    int srcW = FF_CEIL_RSHIFT(desc->src->width, desc->src->h_chr_sub_sample);
    int dstW = FF_CEIL_RSHIFT(desc->dst->width, desc->dst->h_chr_sub_sample);
    int xInc = instance->xInc;
    uint8_t ** src1 = desc->src->plane[1].line;
    uint8_t ** dst1 = desc->dst->plane[1].line;
    uint8_t ** src2 = desc->src->plane[2].line;
    uint8_t ** dst2 = desc->dst->plane[2].line;
    int src_pos1 = sliceY - desc->src->plane[1].sliceY;
    int dst_pos1 = sliceY - desc->dst->plane[1].sliceY;
    int src_pos2 = sliceY - desc->src->plane[2].sliceY;
    int dst_pos2 = sliceY - desc->dst->plane[2].sliceY;
    int i;
    for (i = 0; i < sliceH; ++i) {
        if (c->hcscale_fast) {
            c->hcscale_fast(c, (uint16_t*)dst1[dst_pos1+i], (uint16_t*)dst2[dst_pos2+i], dstW, src1[src_pos1+i], src2[src_pos2+i], srcW, xInc);
        } else {
            c->hcScale(c, (uint16_t*)dst1[dst_pos1+i], dstW, src1[src_pos1+i], instance->filter, instance->filter_pos, instance->filter_size);
            c->hcScale(c, (uint16_t*)dst2[dst_pos2+i], dstW, src2[src_pos2+i], instance->filter, instance->filter_pos, instance->filter_size);
        }
        if (c->chrConvertRange)
            c->chrConvertRange((uint16_t*)dst1[dst_pos1+i], (uint16_t*)dst2[dst_pos2+i], dstW);
        desc->dst->plane[1].sliceH += 1;
        desc->dst->plane[2].sliceH += 1;
    }
    return sliceH;
}
