static int reduce_formats_on_filter(AVFilterContext *filter)
{
    int i, j, k, ret = 0;
    REDUCE_FORMATS(int,      AVFilterFormats,        formats,         formats,
                   nb_formats, ff_add_format);
    REDUCE_FORMATS(int,      AVFilterFormats,        samplerates,     formats,
                   nb_formats, ff_add_format);
    
    for (i = 0; i < filter->nb_inputs; i++) {
        AVFilterLink *inlink = filter->inputs[i];
        uint64_t fmt;
        if (!inlink->out_channel_layouts ||
            inlink->out_channel_layouts->nb_channel_layouts != 1)
            continue;
        fmt = inlink->out_channel_layouts->channel_layouts[0];
        for (j = 0; j < filter->nb_outputs; j++) {
            AVFilterLink *outlink = filter->outputs[j];
            AVFilterChannelLayouts *fmts;
            fmts = outlink->in_channel_layouts;
            if (inlink->type != outlink->type || fmts->nb_channel_layouts == 1)
                continue;
            if (fmts->all_layouts &&
                (!FF_LAYOUT2COUNT(fmt) || fmts->all_counts)) {
                
                fmts->all_layouts = fmts->all_counts  = 0;
                ff_add_channel_layout(&outlink->in_channel_layouts, fmt);
                break;
            }
            for (k = 0; k < outlink->in_channel_layouts->nb_channel_layouts; k++) {
                if (fmts->channel_layouts[k] == fmt) {
                    fmts->channel_layouts[0]  = fmt;
                    fmts->nb_channel_layouts = 1;
                    ret = 1;
                    break;
                }
            }
        }
    }
    return ret;
}
