static int query_formats(AVFilterContext *ctx)
{
    int i;
    AVFilterFormats *formats, *rates;
    AVFilterChannelLayouts *layouts;
    for (i = 0; i < 2; i++) {
        formats = ctx->inputs[i]->in_formats;
        ff_formats_ref(formats, &ctx->inputs[i]->out_formats);
        ff_formats_ref(formats, &ctx->outputs[i]->in_formats);
        rates = ff_all_samplerates();
        ff_formats_ref(rates, &ctx->inputs[i]->out_samplerates);
        ff_formats_ref(rates, &ctx->outputs[i]->in_samplerates);
        layouts = ctx->inputs[i]->in_channel_layouts;
        ff_channel_layouts_ref(layouts, &ctx->inputs[i]->out_channel_layouts);
        ff_channel_layouts_ref(layouts, &ctx->outputs[i]->in_channel_layouts);
    }
    return 0;
}
