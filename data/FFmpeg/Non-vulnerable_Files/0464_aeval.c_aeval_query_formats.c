AVFILTER_DEFINE_CLASS(aeval);
static int aeval_query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layouts;
    AVFilterLink *inlink  = ctx->inputs[0];
    AVFilterLink *outlink  = ctx->outputs[0];
    EvalContext *eval = ctx->priv;
    static const enum AVSampleFormat sample_fmts[] = {
        AV_SAMPLE_FMT_DBLP, AV_SAMPLE_FMT_NONE
    };
    
    layouts = ff_all_channel_counts();
    ff_channel_layouts_ref(layouts, &inlink->out_channel_layouts);
    if (eval->same_chlayout) {
        layouts = ff_all_channel_counts();
        if (!layouts)
            return AVERROR(ENOMEM);
            ff_set_common_channel_layouts(ctx, layouts);
    } else {
        
        layouts = NULL;
        ff_add_channel_layout(&layouts,
                              eval->out_channel_layout ? eval->out_channel_layout :
                              FF_COUNT2LAYOUT(eval->nb_channels));
        ff_channel_layouts_ref(layouts, &outlink->in_channel_layouts);
    }
    formats = ff_make_format_list(sample_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_set_common_formats(ctx, formats);
    formats = ff_all_samplerates();
    if (!formats)
        return AVERROR(ENOMEM);
    ff_set_common_samplerates(ctx, formats);
    return 0;
}
