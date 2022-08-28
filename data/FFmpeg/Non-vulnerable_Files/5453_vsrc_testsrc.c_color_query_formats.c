static int color_query_formats(AVFilterContext *ctx)
{
    return ff_set_common_formats(ctx, ff_draw_supported_pixel_formats(0));
}
