static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats;
    AVFilterChannelLayouts *layouts = NULL;
    static const enum AVSampleFormat sample_fmts[] = {
        AV_SAMPLE_FMT_DBL,
        AV_SAMPLE_FMT_NONE
    };
    int ret, i;
    if (!ctx->inputs[0]->in_channel_layouts ||
        !ctx->inputs[0]->in_channel_layouts->nb_channel_layouts) {
        av_log(ctx, AV_LOG_WARNING,
               "No channel layout for input 1\n");
            return AVERROR(EAGAIN);
    }
    ff_add_channel_layout(&layouts, ctx->inputs[0]->in_channel_layouts->channel_layouts[0]);
    if (!layouts)
        return AVERROR(ENOMEM);
    ff_channel_layouts_ref(layouts, &ctx->outputs[0]->in_channel_layouts);
    for (i = 0; i < 2; i++) {
        layouts = ff_all_channel_layouts();
        if (!layouts)
            return AVERROR(ENOMEM);
        ff_channel_layouts_ref(layouts, &ctx->inputs[i]->out_channel_layouts);
    }
    formats = ff_make_format_list(sample_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ret = ff_set_common_formats(ctx, formats);
    if (ret < 0)
        return ret;
    formats = ff_all_samplerates();
    if (!formats)
        return AVERROR(ENOMEM);
    return ff_set_common_samplerates(ctx, formats);
}
