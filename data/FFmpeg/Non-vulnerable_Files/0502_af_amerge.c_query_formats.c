static int query_formats(AVFilterContext *ctx)
{
    AMergeContext *s = ctx->priv;
    int64_t inlayout[SWR_CH_MAX], outlayout = 0;
    AVFilterFormats *formats;
    AVFilterChannelLayouts *layouts;
    int i, overlap = 0, nb_ch = 0;
    for (i = 0; i < s->nb_inputs; i++) {
        if (!ctx->inputs[i]->in_channel_layouts ||
            !ctx->inputs[i]->in_channel_layouts->nb_channel_layouts) {
            av_log(ctx, AV_LOG_WARNING,
                   "No channel layout for input %d\n", i + 1);
            return AVERROR(EAGAIN);
        }
        inlayout[i] = ctx->inputs[i]->in_channel_layouts->channel_layouts[0];
        if (ctx->inputs[i]->in_channel_layouts->nb_channel_layouts > 1) {
            char buf[256];
            av_get_channel_layout_string(buf, sizeof(buf), 0, inlayout[i]);
            av_log(ctx, AV_LOG_INFO, "Using \"%s\" for input %d\n", buf, i + 1);
        }
        s->in[i].nb_ch = av_get_channel_layout_nb_channels(inlayout[i]);
        if (outlayout & inlayout[i])
            overlap++;
        outlayout |= inlayout[i];
        nb_ch += s->in[i].nb_ch;
    }
    if (nb_ch > SWR_CH_MAX) {
        av_log(ctx, AV_LOG_ERROR, "Too many channels (max %d)\n", SWR_CH_MAX);
        return AVERROR(EINVAL);
    }
    if (overlap) {
        av_log(ctx, AV_LOG_WARNING,
               "Input channel layouts overlap: "
               "output layout will be determined by the number of distinct input channels\n");
        for (i = 0; i < nb_ch; i++)
            s->route[i] = i;
        outlayout = av_get_default_channel_layout(nb_ch);
        if (!outlayout && nb_ch)
            outlayout = 0xFFFFFFFFFFFFFFFFULL >> (64 - nb_ch);
    } else {
        int *route[SWR_CH_MAX];
        int c, out_ch_number = 0;
        route[0] = s->route;
        for (i = 1; i < s->nb_inputs; i++)
            route[i] = route[i - 1] + s->in[i - 1].nb_ch;
        for (c = 0; c < 64; c++)
            for (i = 0; i < s->nb_inputs; i++)
                if ((inlayout[i] >> c) & 1)
                    *(route[i]++) = out_ch_number++;
    }
    formats = ff_make_format_list(ff_packed_sample_fmts_array);
    ff_set_common_formats(ctx, formats);
    for (i = 0; i < s->nb_inputs; i++) {
        layouts = NULL;
        ff_add_channel_layout(&layouts, inlayout[i]);
        ff_channel_layouts_ref(layouts, &ctx->inputs[i]->out_channel_layouts);
    }
    layouts = NULL;
    ff_add_channel_layout(&layouts, outlayout);
    ff_channel_layouts_ref(layouts, &ctx->outputs[0]->in_channel_layouts);
    ff_set_common_samplerates(ctx, ff_all_samplerates());
    return 0;
}
