int ff_init_gamma_convert(SwsFilterDescriptor *desc, SwsSlice * src, uint16_t *table)
{
    GammaContext *li = av_malloc(sizeof(GammaContext));
    if (!li)
        return AVERROR(ENOMEM);
    li->table = table;
    desc->instance = li;
    desc->src = src;
    desc->dst = NULL;
    desc->process = &gamma_convert;
    return 0;
}
