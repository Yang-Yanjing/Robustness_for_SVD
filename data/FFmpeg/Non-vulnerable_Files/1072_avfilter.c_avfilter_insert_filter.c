int avfilter_insert_filter(AVFilterLink *link, AVFilterContext *filt,
                           unsigned filt_srcpad_idx, unsigned filt_dstpad_idx)
{
    int ret;
    unsigned dstpad_idx = link->dstpad - link->dst->input_pads;
    av_log(link->dst, AV_LOG_VERBOSE, "auto-inserting filter '%s' "
           "between the filter '%s' and the filter '%s'\n",
           filt->name, link->src->name, link->dst->name);
    link->dst->inputs[dstpad_idx] = NULL;
    if ((ret = avfilter_link(filt, filt_dstpad_idx, link->dst, dstpad_idx)) < 0) {
        
        link->dst->inputs[dstpad_idx] = link;
        return ret;
    }
    
    link->dst                     = filt;
    link->dstpad                  = &filt->input_pads[filt_srcpad_idx];
    filt->inputs[filt_srcpad_idx] = link;
    

    if (link->out_formats)
        ff_formats_changeref(&link->out_formats,
                             &filt->outputs[filt_dstpad_idx]->out_formats);
    if (link->out_samplerates)
        ff_formats_changeref(&link->out_samplerates,
                             &filt->outputs[filt_dstpad_idx]->out_samplerates);
    if (link->out_channel_layouts)
        ff_channel_layouts_changeref(&link->out_channel_layouts,
                                     &filt->outputs[filt_dstpad_idx]->out_channel_layouts);
    return 0;
}
