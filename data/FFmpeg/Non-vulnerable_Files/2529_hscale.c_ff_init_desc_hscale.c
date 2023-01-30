int ff_init_desc_hscale(SwsFilterDescriptor *desc, SwsSlice *src, SwsSlice *dst, uint16_t *filter, int * filter_pos, int filter_size, int xInc)
{
    FilterContext *li = av_malloc(sizeof(FilterContext));
    if (!li)
        return AVERROR(ENOMEM);
    li->filter = filter;
    li->filter_pos = filter_pos;
    li->filter_size = filter_size;
    li->xInc = xInc;
    desc->instance = li;
    desc->alpha = isALPHA(src->fmt) && isALPHA(dst->fmt);
    desc->src = src;
    desc->dst = dst;
    desc->process = &lum_h_scale;
    return 0;
}
