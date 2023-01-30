int ff_init_desc_cfmt_convert(SwsFilterDescriptor *desc, SwsSlice * src, SwsSlice *dst, uint32_t *pal)
{
    ColorContext * li = av_malloc(sizeof(ColorContext));
    if (!li)
        return AVERROR(ENOMEM);
    li->pal = pal;
    desc->instance = li;
    desc->src =src;
    desc->dst = dst;
    desc->process = &chr_convert;
    return 0;
}
