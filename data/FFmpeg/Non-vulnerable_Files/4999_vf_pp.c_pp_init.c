AVFILTER_DEFINE_CLASS(pp);
static av_cold int pp_init(AVFilterContext *ctx)
{
    int i;
    PPFilterContext *pp = ctx->priv;
    for (i = 0; i <= PP_QUALITY_MAX; i++) {
        pp->modes[i] = pp_get_mode_by_name_and_quality(pp->subfilters, i);
        if (!pp->modes[i])
            return AVERROR_EXTERNAL;
    }
    pp->mode_id = PP_QUALITY_MAX;
    return 0;
}
