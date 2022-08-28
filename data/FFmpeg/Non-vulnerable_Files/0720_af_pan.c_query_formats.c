static int query_formats(AVFilterContext *ctx)
{
    PanContext *pan = ctx->priv;
    AVFilterLink *inlink  = ctx->inputs[0];
    AVFilterLink *outlink = ctx->outputs[0];
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layouts;
    pan->pure_gains = are_gains_pure(pan);
    
    ff_set_common_formats(ctx, ff_all_formats(AVMEDIA_TYPE_AUDIO));
    formats = ff_all_samplerates();
    if (!formats)
        return AVERROR(ENOMEM);
    ff_set_common_samplerates(ctx, formats);
    
    layouts = ff_all_channel_counts();
    ff_channel_layouts_ref(layouts, &inlink->out_channel_layouts);
    
    layouts = NULL;
    ff_add_channel_layout(&layouts,
                          pan->out_channel_layout ? pan->out_channel_layout :
                          FF_COUNT2LAYOUT(pan->nb_output_channels));
    ff_channel_layouts_ref(layouts, &outlink->in_channel_layouts);
    return 0;
}
