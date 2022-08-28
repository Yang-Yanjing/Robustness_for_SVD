static int query_formats(AVFilterContext *ctx)
{
    AVFilterChannelLayouts *layouts = NULL;
    AVFilterFormats        *formats = NULL;
    
    
    
    
    
    
    static const enum AVSampleFormat sample_fmts[] = {
        AV_SAMPLE_FMT_U8,
        AV_SAMPLE_FMT_S16,
        AV_SAMPLE_FMT_S32,
        AV_SAMPLE_FMT_FLT,
        AV_SAMPLE_FMT_DBL,
        AV_SAMPLE_FMT_NONE
    };
    int ret;
    layouts = ff_all_channel_layouts();
    if (!layouts) {
        return AVERROR(ENOMEM);
    }
    ret = ff_set_common_channel_layouts(ctx, layouts);
    if (ret < 0)
        return ret;
    formats = ff_make_format_list(sample_fmts);
    if (!formats) {
        return AVERROR(ENOMEM);
    }
    ret = ff_set_common_formats(ctx, formats);
    if (ret < 0)
        return ret;
    formats = ff_all_samplerates();
    if (!formats) {
        return AVERROR(ENOMEM);
    }
    return ff_set_common_samplerates(ctx, formats);
}
