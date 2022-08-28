AVFilterChannelLayouts *ff_merge_channel_layouts(AVFilterChannelLayouts *a,
                                                 AVFilterChannelLayouts *b)
{
    AVFilterChannelLayouts *ret = NULL;
    unsigned a_all = a->all_layouts + a->all_counts;
    unsigned b_all = b->all_layouts + b->all_counts;
    int ret_max, ret_nb = 0, i, j, round;
    if (a == b) return a;
    
    if (a_all < b_all) {
        FFSWAP(AVFilterChannelLayouts *, a, b);
        FFSWAP(unsigned, a_all, b_all);
    }
    if (a_all) {
        if (a_all == 1 && !b_all) {
            
            for (i = j = 0; i < b->nb_channel_layouts; i++)
                if (KNOWN(b->channel_layouts[i]))
                    b->channel_layouts[j++] = b->channel_layouts[i];
            

            if (!j)
                return NULL;
            b->nb_channel_layouts = j;
        }
        MERGE_REF(b, a, channel_layouts, AVFilterChannelLayouts, fail);
        return b;
    }
    ret_max = a->nb_channel_layouts + b->nb_channel_layouts;
    if (!(ret = av_mallocz(sizeof(*ret))) ||
        !(ret->channel_layouts = av_malloc_array(ret_max,
                                                 sizeof(*ret->channel_layouts))))
        goto fail;
    
    for (i = 0; i < a->nb_channel_layouts; i++) {
        if (!KNOWN(a->channel_layouts[i]))
            continue;
        for (j = 0; j < b->nb_channel_layouts; j++) {
            if (a->channel_layouts[i] == b->channel_layouts[j]) {
                ret->channel_layouts[ret_nb++] = a->channel_layouts[i];
                a->channel_layouts[i] = b->channel_layouts[j] = 0;
            }
        }
    }
    

    for (round = 0; round < 2; round++) {
        for (i = 0; i < a->nb_channel_layouts; i++) {
            uint64_t fmt = a->channel_layouts[i], bfmt;
            if (!fmt || !KNOWN(fmt))
                continue;
            bfmt = FF_COUNT2LAYOUT(av_get_channel_layout_nb_channels(fmt));
            for (j = 0; j < b->nb_channel_layouts; j++)
                if (b->channel_layouts[j] == bfmt)
                    ret->channel_layouts[ret_nb++] = a->channel_layouts[i];
        }
        
        FFSWAP(AVFilterChannelLayouts *, a, b);
    }
    
    for (i = 0; i < a->nb_channel_layouts; i++) {
        if (KNOWN(a->channel_layouts[i]))
            continue;
        for (j = 0; j < b->nb_channel_layouts; j++)
            if (a->channel_layouts[i] == b->channel_layouts[j])
                ret->channel_layouts[ret_nb++] = a->channel_layouts[i];
    }
    ret->nb_channel_layouts = ret_nb;
    if (!ret->nb_channel_layouts)
        goto fail;
    MERGE_REF(ret, a, channel_layouts, AVFilterChannelLayouts, fail);
    MERGE_REF(ret, b, channel_layouts, AVFilterChannelLayouts, fail);
    return ret;
fail:
    if (ret) {
        av_freep(&ret->refs);
        av_freep(&ret->channel_layouts);
    }
    av_freep(&ret);
    return NULL;
}
