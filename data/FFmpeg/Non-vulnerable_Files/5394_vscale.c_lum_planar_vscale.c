static int lum_planar_vscale(SwsContext *c, SwsFilterDescriptor *desc, int sliceY, int sliceH)
{
    VScalerContext *inst = desc->instance;
    int dstW = desc->dst->width;
    int first = FFMAX(1-inst->filter_size, inst->filter_pos[sliceY]);
    int sp = first - desc->src->plane[0].sliceY;
    int dp = sliceY - desc->dst->plane[0].sliceY;
    uint8_t **src = desc->src->plane[0].line + sp;
    uint8_t **dst = desc->dst->plane[0].line + dp;
    uint16_t *filter = inst->filter[0] + (inst->isMMX ? 0 : sliceY * inst->filter_size);
    if (inst->filter_size == 1)
        ((yuv2planar1_fn)inst->pfn)((const int16_t*)src[0], dst[0], dstW, c->lumDither8, 0);
    else
        ((yuv2planarX_fn)inst->pfn)(filter, inst->filter_size, (const int16_t**)src, dst[0], dstW, c->lumDither8, 0);
    if (desc->alpha) {
        int sp = first - desc->src->plane[3].sliceY;
        int dp = sliceY - desc->dst->plane[3].sliceY;
        uint8_t **src = desc->src->plane[3].line + sp;
        uint8_t **dst = desc->dst->plane[3].line + dp;
        uint16_t *filter = inst->filter[1] + (inst->isMMX ? 0 : sliceY * inst->filter_size);
        if (inst->filter_size == 1)
            ((yuv2planar1_fn)inst->pfn)((const int16_t*)src[0], dst[0], dstW, c->lumDither8, 0);
        else
            ((yuv2planarX_fn)inst->pfn)(filter, inst->filter_size, (const int16_t**)src, dst[0], dstW, c->lumDither8, 0);
    }
    return 1;
}
