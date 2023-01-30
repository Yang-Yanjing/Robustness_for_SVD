AVFILTER_DEFINE_CLASS(find_rect);
static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_YUV420P,
        AV_PIX_FMT_YUVJ420P,
        AV_PIX_FMT_NONE
    };
    return ff_set_common_formats(ctx, ff_make_format_list(pix_fmts));
}
