int ff_init_desc_no_chr(SwsFilterDescriptor *desc, SwsSlice * src, SwsSlice *dst)
{
    desc->src = src;
    desc->dst = dst;
    desc->alpha = 0;
    desc->instance = NULL;
    desc->process = &no_chr_scale;
    return 0;
}
