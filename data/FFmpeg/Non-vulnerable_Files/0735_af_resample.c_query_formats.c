static int query_formats(AVFilterContext *ctx)
{
    AVFilterLink *inlink  = ctx->inputs[0];
    AVFilterLink *outlink = ctx->outputs[0];
    AVFilterFormats        *in_formats      = ff_all_formats(AVMEDIA_TYPE_AUDIO);
    AVFilterFormats        *out_formats     = ff_all_formats(AVMEDIA_TYPE_AUDIO);
    AVFilterFormats        *in_samplerates  = ff_all_samplerates();
    AVFilterFormats        *out_samplerates = ff_all_samplerates();
    AVFilterChannelLayouts *in_layouts      = ff_all_channel_layouts();
    AVFilterChannelLayouts *out_layouts     = ff_all_channel_layouts();
    ff_formats_ref(in_formats,  &inlink->out_formats);
    ff_formats_ref(out_formats, &outlink->in_formats);
    ff_formats_ref(in_samplerates,  &inlink->out_samplerates);
    ff_formats_ref(out_samplerates, &outlink->in_samplerates);
    ff_channel_layouts_ref(in_layouts,  &inlink->out_channel_layouts);
    ff_channel_layouts_ref(out_layouts, &outlink->in_channel_layouts);
    return 0;
}
