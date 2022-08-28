static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layouts = NULL;
    AVFilterLink *inlink = ctx->inputs[0];
    AVFilterLink *outlink = ctx->outputs[0];
    static const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_NONE };
    static const enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_RGB24, AV_PIX_FMT_NONE };
    static const int64_t channel_layouts[] = { AV_CH_LAYOUT_STEREO, AV_CH_LAYOUT_STEREO_DOWNMIX, -1 };
    static const int samplerates[] = { 44100, 48000, -1 };
    
    formats = ff_make_format_list(sample_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &inlink->out_formats);
    layouts = avfilter_make_format64_list(channel_layouts);
    if (!layouts)
        return AVERROR(ENOMEM);
    ff_channel_layouts_ref(layouts, &inlink->out_channel_layouts);
    formats = ff_make_format_list(samplerates);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &inlink->out_samplerates);
    
    formats = ff_make_format_list(pix_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &outlink->in_formats);
    return 0;
}
