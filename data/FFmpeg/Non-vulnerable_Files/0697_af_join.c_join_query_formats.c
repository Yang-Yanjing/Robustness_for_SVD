static int join_query_formats(AVFilterContext *ctx)
{
    JoinContext *s = ctx->priv;
    AVFilterChannelLayouts *layouts = NULL;
    int i;
    ff_add_channel_layout(&layouts, s->channel_layout);
    ff_channel_layouts_ref(layouts, &ctx->outputs[0]->in_channel_layouts);
    for (i = 0; i < ctx->nb_inputs; i++) {
        layouts = ff_all_channel_layouts();
        if (!layouts)
            return AVERROR(ENOMEM);
        ff_channel_layouts_ref(layouts, &ctx->inputs[i]->out_channel_layouts);
    }
    ff_set_common_formats    (ctx, ff_planar_sample_fmts());
    ff_set_common_samplerates(ctx, ff_all_samplerates());
    return 0;
}
