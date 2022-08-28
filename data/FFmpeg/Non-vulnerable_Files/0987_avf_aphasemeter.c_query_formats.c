AVFILTER_DEFINE_CLASS(aphasemeter);
static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layout = NULL;
    AVFilterLink *inlink = ctx->inputs[0];
    AVFilterLink *outlink = ctx->outputs[0];
    static const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_NONE };
    static const enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_RGBA, AV_PIX_FMT_NONE };
    formats = ff_make_format_list(sample_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &inlink->out_formats);
    ff_add_channel_layout(&layout, AV_CH_LAYOUT_STEREO);
    ff_channel_layouts_ref(layout, &inlink->out_channel_layouts);
    formats = ff_all_samplerates();
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &inlink->out_samplerates);
    formats = ff_make_format_list(pix_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &outlink->in_formats);
    return 0;
}
