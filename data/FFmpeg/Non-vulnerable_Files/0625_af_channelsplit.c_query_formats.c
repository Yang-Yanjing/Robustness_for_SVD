static int query_formats(AVFilterContext *ctx)
{
    ChannelSplitContext *s = ctx->priv;
    AVFilterChannelLayouts *in_layouts = NULL;
    int i;
    ff_set_common_formats    (ctx, ff_planar_sample_fmts());
    ff_set_common_samplerates(ctx, ff_all_samplerates());
    ff_add_channel_layout(&in_layouts, s->channel_layout);
    ff_channel_layouts_ref(in_layouts, &ctx->inputs[0]->out_channel_layouts);
    for (i = 0; i < ctx->nb_outputs; i++) {
        AVFilterChannelLayouts *out_layouts = NULL;
        uint64_t channel = av_channel_layout_extract_channel(s->channel_layout, i);
        ff_add_channel_layout(&out_layouts, channel);
        ff_channel_layouts_ref(out_layouts, &ctx->outputs[i]->in_channel_layouts);
    }
    return 0;
}
