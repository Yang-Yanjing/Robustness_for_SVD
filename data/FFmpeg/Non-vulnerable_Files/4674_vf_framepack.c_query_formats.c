static int query_formats(AVFilterContext *ctx)
{
    
    AVFilterFormats *fmts_list = ff_make_format_list(formats_supported);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
