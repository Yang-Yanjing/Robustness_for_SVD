int ff_init_vscale(SwsContext *c, SwsFilterDescriptor *desc, SwsSlice *src, SwsSlice *dst)
{
    VScalerContext *lumCtx = NULL;
    VScalerContext *chrCtx = NULL;
    if (isPlanarYUV(c->dstFormat) || (isGray(c->dstFormat) && !isALPHA(c->dstFormat))) {
        lumCtx = av_mallocz(sizeof(VScalerContext));
        if (!lumCtx)
            return AVERROR(ENOMEM);
        desc[0].process = lum_planar_vscale;
        desc[0].instance = lumCtx;
        desc[0].src = src;
        desc[0].dst = dst;
        desc[0].alpha = c->alpPixBuf != 0;
        if (!isGray(c->dstFormat)) {
            chrCtx = av_mallocz(sizeof(VScalerContext));
            if (!chrCtx)
                return AVERROR(ENOMEM);
            desc[1].process = chr_planar_vscale;
            desc[1].instance = chrCtx;
            desc[1].src = src;
            desc[1].dst = dst;
        }
    } else {
        lumCtx = av_mallocz_array(sizeof(VScalerContext), 2);
        if (!lumCtx)
            return AVERROR(ENOMEM);
        chrCtx = &lumCtx[1];
        desc[0].process = c->yuv2packedX ? packed_vscale : any_vscale;
        desc[0].instance = lumCtx;
        desc[0].src = src;
        desc[0].dst = dst;
        desc[0].alpha = c->alpPixBuf != 0;
    }
    ff_init_vscale_pfn(c, c->yuv2plane1, c->yuv2planeX, c->yuv2nv12cX,
        c->yuv2packed1, c->yuv2packed2, c->yuv2packedX, c->yuv2anyX, c->use_mmx_vfilter);
    return 0;
}
