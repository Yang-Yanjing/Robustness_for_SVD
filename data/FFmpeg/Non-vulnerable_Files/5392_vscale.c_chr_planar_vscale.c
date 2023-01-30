static int chr_planar_vscale(SwsContext *c, SwsFilterDescriptor *desc, int sliceY, int sliceH)
{
    const int chrSkipMask = (1 << desc->dst->v_chr_sub_sample) - 1;
    if (sliceY & chrSkipMask)
        return 0;
    else {
        VScalerContext *inst = desc->instance;
        int dstW = FF_CEIL_RSHIFT(desc->dst->width, desc->dst->h_chr_sub_sample);
        int chrSliceY = sliceY >> desc->dst->v_chr_sub_sample;
        int first = FFMAX(1-inst->filter_size, inst->filter_pos[chrSliceY]);
        int sp1 = first - desc->src->plane[1].sliceY;
        int sp2 = first - desc->src->plane[2].sliceY;
        int dp1 = chrSliceY - desc->dst->plane[1].sliceY;
        int dp2 = chrSliceY - desc->dst->plane[2].sliceY;
        uint8_t **src1 = desc->src->plane[1].line + sp1;
        uint8_t **src2 = desc->src->plane[2].line + sp2;
        uint8_t **dst1 = desc->dst->plane[1].line + dp1;
        uint8_t **dst2 = desc->dst->plane[2].line + dp2;
        uint16_t *filter = inst->filter[0] + (inst->isMMX ? 0 : chrSliceY * inst->filter_size);
        if (c->yuv2nv12cX) {
            ((yuv2interleavedX_fn)inst->pfn)(c, filter, inst->filter_size, (const int16_t**)src1, (const int16_t**)src2, dst1[0], dstW);
        } else if (inst->filter_size == 1) {
            ((yuv2planar1_fn)inst->pfn)((const int16_t*)src1[0], dst1[0], dstW, c->chrDither8, 0);
            ((yuv2planar1_fn)inst->pfn)((const int16_t*)src2[0], dst2[0], dstW, c->chrDither8, 3);
        } else {
            ((yuv2planarX_fn)inst->pfn)(filter, inst->filter_size, (const int16_t**)src1, dst1[0], dstW, c->chrDither8, 0);
            ((yuv2planarX_fn)inst->pfn)(filter, inst->filter_size, (const int16_t**)src2, dst2[0], dstW, c->chrDither8, inst->isMMX ? (c->uv_offx2 >> 1) : 3);
        }
    }
    return 1;
}
