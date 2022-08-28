static int query_formats(AVFilterContext *ctx)
{
    static const enum AVSampleFormat sample_fmts[] = {
        AV_SAMPLE_FMT_S16,
        AV_SAMPLE_FMT_S16P,
        AV_SAMPLE_FMT_NONE
    };
    AVFilterFormats *formats;
    if (!(formats = ff_make_format_list(sample_fmts)))
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, formats);
}
