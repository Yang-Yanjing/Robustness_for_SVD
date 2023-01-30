static int query_formats(AVFilterContext *ctx)
{
    FormatContext *s = ctx->priv;
    AVFilterFormats *formats = ff_make_format_list(s->formats);
    if (!formats)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, formats);
}
