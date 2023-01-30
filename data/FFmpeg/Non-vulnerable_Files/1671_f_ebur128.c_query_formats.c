static int query_formats(AVFilterContext *ctx)
{
    EBUR128Context *ebur128 = ctx->priv;
    AVFilterFormats *formats;
    AVFilterChannelLayouts *layouts;
    AVFilterLink *inlink = ctx->inputs[0];
    AVFilterLink *outlink = ctx->outputs[0];
    static const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_DBL, AV_SAMPLE_FMT_NONE };
    static const int input_srate[] = {48000, -1}; 
    static const enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_RGB24, AV_PIX_FMT_NONE };
    
    if (ebur128->do_video) {
        formats = ff_make_format_list(pix_fmts);
        if (!formats)
            return AVERROR(ENOMEM);
        ff_formats_ref(formats, &outlink->in_formats);
        outlink = ctx->outputs[1];
    }
    


    formats = ff_make_format_list(sample_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &inlink->out_formats);
    ff_formats_ref(formats, &outlink->in_formats);
    layouts = ff_all_channel_layouts();
    if (!layouts)
        return AVERROR(ENOMEM);
    ff_channel_layouts_ref(layouts, &inlink->out_channel_layouts);
    ff_channel_layouts_ref(layouts, &outlink->in_channel_layouts);
    formats = ff_make_format_list(input_srate);
    if (!formats)
        return AVERROR(ENOMEM);
    ff_formats_ref(formats, &inlink->out_samplerates);
    ff_formats_ref(formats, &outlink->in_samplerates);
    return 0;
}
