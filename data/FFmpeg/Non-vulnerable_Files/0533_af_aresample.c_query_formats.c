static int query_formats(AVFilterContext *ctx)
{
    AResampleContext *aresample = ctx->priv;
    enum AVSampleFormat out_format;
    int64_t out_rate, out_layout;
    AVFilterLink *inlink  = ctx->inputs[0];
    AVFilterLink *outlink = ctx->outputs[0];
    AVFilterFormats        *in_formats, *out_formats;
    AVFilterFormats        *in_samplerates, *out_samplerates;
    AVFilterChannelLayouts *in_layouts, *out_layouts;
    av_opt_get_sample_fmt(aresample->swr, "osf", 0, &out_format);
    av_opt_get_int(aresample->swr, "osr", 0, &out_rate);
    av_opt_get_int(aresample->swr, "ocl", 0, &out_layout);
    in_formats      = ff_all_formats(AVMEDIA_TYPE_AUDIO);
    if (!in_formats)
        return AVERROR(ENOMEM);
    ff_formats_ref  (in_formats,      &inlink->out_formats);
    in_samplerates  = ff_all_samplerates();
    if (!in_samplerates)
        return AVERROR(ENOMEM);
    ff_formats_ref  (in_samplerates,  &inlink->out_samplerates);
    in_layouts      = ff_all_channel_counts();
    if (!in_layouts)
         return AVERROR(ENOMEM);
    ff_channel_layouts_ref(in_layouts,      &inlink->out_channel_layouts);
    if(out_rate > 0) {
        int ratelist[] = { out_rate, -1 };
        out_samplerates = ff_make_format_list(ratelist);
    } else {
        out_samplerates = ff_all_samplerates();
    }
    if (!out_samplerates) {
        av_log(ctx, AV_LOG_ERROR, "Cannot allocate output samplerates.\n");
        return AVERROR(ENOMEM);
    }
    ff_formats_ref(out_samplerates, &outlink->in_samplerates);
    if(out_format != AV_SAMPLE_FMT_NONE) {
        int formatlist[] = { out_format, -1 };
        out_formats = ff_make_format_list(formatlist);
    } else
        out_formats = ff_all_formats(AVMEDIA_TYPE_AUDIO);
    ff_formats_ref(out_formats, &outlink->in_formats);
    if(out_layout) {
        int64_t layout_list[] = { out_layout, -1 };
        out_layouts = avfilter_make_format64_list(layout_list);
    } else
        out_layouts = ff_all_channel_counts();
    ff_channel_layouts_ref(out_layouts, &outlink->in_channel_layouts);
    return 0;
}
