static av_cold int query_formats(AVFilterContext *avctx)
{
    static const enum AVPixelFormat pixel_fmts[] = {
        AV_PIX_FMT_ARGB,
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_ABGR,
        AV_PIX_FMT_BGRA,
        AV_PIX_FMT_NONE
    };
    AVFilterFormats *formats = NULL;
    formats = ff_make_format_list(pixel_fmts);
    if (!formats)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(avctx, formats);
}
