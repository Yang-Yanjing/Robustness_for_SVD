int ff_set_common_formats(AVFilterContext *ctx, AVFilterFormats *formats)
{
    SET_COMMON_FORMATS(ctx, formats, in_formats, out_formats,
                       ff_formats_ref, formats);
}
