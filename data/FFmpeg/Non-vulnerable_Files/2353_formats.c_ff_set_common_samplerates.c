int ff_set_common_samplerates(AVFilterContext *ctx,
                              AVFilterFormats *samplerates)
{
    SET_COMMON_FORMATS(ctx, samplerates, in_samplerates, out_samplerates,
                       ff_formats_ref, formats);
}
