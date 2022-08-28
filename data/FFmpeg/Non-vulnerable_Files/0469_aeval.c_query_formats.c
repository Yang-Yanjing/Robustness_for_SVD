static int query_formats(AVFilterContext *ctx)
{
    EvalContext *eval = ctx->priv;
    static const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_DBLP, AV_SAMPLE_FMT_NONE };
    int64_t chlayouts[] = { eval->chlayout ? eval->chlayout : FF_COUNT2LAYOUT(eval->nb_channels) , -1 };
    int sample_rates[] = { eval->sample_rate, -1 };
    AVFilterFormats *formats;
    AVFilterChannelLayouts *layouts;
    int ret;
    formats = ff_make_format_list(sample_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    ret = ff_set_common_formats (ctx, formats);
    if (ret < 0)
        return ret;
    layouts = avfilter_make_format64_list(chlayouts);
    if (!layouts)
        return AVERROR(ENOMEM);
    ret = ff_set_common_channel_layouts(ctx, layouts);
    if (ret < 0)
        return ret;
    formats = ff_make_format_list(sample_rates);
    if (!formats)
        return AVERROR(ENOMEM);
    return ff_set_common_samplerates(ctx, formats);
}
