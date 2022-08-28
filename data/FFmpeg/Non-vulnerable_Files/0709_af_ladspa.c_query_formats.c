static int query_formats(AVFilterContext *ctx)
{
    LADSPAContext *s = ctx->priv;
    AVFilterFormats *formats;
    AVFilterChannelLayouts *layouts;
    static const enum AVSampleFormat sample_fmts[] = {
        AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_NONE };
    formats = ff_make_format_list(sample_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_set_common_formats(ctx, formats);
    if (s->nb_inputs) {
        formats = ff_all_samplerates();
        if (!formats)
            return AVERROR(ENOMEM);
        ff_set_common_samplerates(ctx, formats);
    } else {
        int sample_rates[] = { s->sample_rate, -1 };
        ff_set_common_samplerates(ctx, ff_make_format_list(sample_rates));
    }
    if (s->nb_inputs == 1 && s->nb_outputs == 1) {
        
        layouts = ff_all_channel_layouts();
        if (!layouts)
            return AVERROR(ENOMEM);
        ff_set_common_channel_layouts(ctx, layouts);
    } else {
        AVFilterLink *outlink = ctx->outputs[0];
        if (s->nb_inputs >= 1) {
            AVFilterLink *inlink = ctx->inputs[0];
            int64_t inlayout = FF_COUNT2LAYOUT(s->nb_inputs);
            layouts = NULL;
            ff_add_channel_layout(&layouts, inlayout);
            ff_channel_layouts_ref(layouts, &inlink->out_channel_layouts);
            if (!s->nb_outputs)
                ff_channel_layouts_ref(layouts, &outlink->in_channel_layouts);
        }
        if (s->nb_outputs >= 1) {
            int64_t outlayout = FF_COUNT2LAYOUT(s->nb_outputs);
            layouts = NULL;
            ff_add_channel_layout(&layouts, outlayout);
            ff_channel_layouts_ref(layouts, &outlink->in_channel_layouts);
        }
    }
    return 0;
}
