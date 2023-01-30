static int query_formats(AVFilterContext *ctx)
{
    VolumeContext *vol = ctx->priv;
    AVFilterFormats *formats = NULL;
    AVFilterChannelLayouts *layouts;
    static const enum AVSampleFormat sample_fmts[][7] = {
        [PRECISION_FIXED] = {
            AV_SAMPLE_FMT_U8,
            AV_SAMPLE_FMT_U8P,
            AV_SAMPLE_FMT_S16,
            AV_SAMPLE_FMT_S16P,
            AV_SAMPLE_FMT_S32,
            AV_SAMPLE_FMT_S32P,
            AV_SAMPLE_FMT_NONE
        },
        [PRECISION_FLOAT] = {
            AV_SAMPLE_FMT_FLT,
            AV_SAMPLE_FMT_FLTP,
            AV_SAMPLE_FMT_NONE
        },
        [PRECISION_DOUBLE] = {
            AV_SAMPLE_FMT_DBL,
            AV_SAMPLE_FMT_DBLP,
            AV_SAMPLE_FMT_NONE
        }
    };
    int ret;
    layouts = ff_all_channel_counts();
    if (!layouts)
        return AVERROR(ENOMEM);
    ret = ff_set_common_channel_layouts(ctx, layouts);
    if (ret < 0)
        return ret;
    formats = ff_make_format_list(sample_fmts[vol->precision]);
    if (!formats)
        return AVERROR(ENOMEM);
    ret = ff_set_common_formats(ctx, formats);
    if (ret < 0)
        return ret;
    formats = ff_all_samplerates();
    if (!formats)
        return AVERROR(ENOMEM);
    return ff_set_common_samplerates(ctx, formats);
}
