int ff_init_desc_fmt_convert(SwsFilterDescriptor *desc, SwsSlice * src, SwsSlice *dst, uint32_t *pal)
{
    ColorContext * li = av_malloc(sizeof(ColorContext));
    if (!li)
        return AVERROR(ENOMEM);
    li->pal = pal;
    desc->instance = li;
    desc->alpha = isALPHA(src->fmt) && isALPHA(dst->fmt);
    desc->src =src;
    desc->dst = dst;
    desc->process = &lum_convert;
    return 0;
}
