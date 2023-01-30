static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_GBRP,   AV_PIX_FMT_GBRAP,
        AV_PIX_FMT_ARGB,   AV_PIX_FMT_RGBA,
        AV_PIX_FMT_ABGR,   AV_PIX_FMT_BGRA,
        AV_PIX_FMT_0RGB,   AV_PIX_FMT_RGB0,
        AV_PIX_FMT_0BGR,   AV_PIX_FMT_BGR0,
        AV_PIX_FMT_RGB24,  AV_PIX_FMT_BGR24,
        AV_PIX_FMT_GRAY8,
        AV_PIX_FMT_YUV410P,
        AV_PIX_FMT_YUV444P,  AV_PIX_FMT_YUVJ444P,
        AV_PIX_FMT_YUV420P,  AV_PIX_FMT_YUVJ420P,
        AV_PIX_FMT_YUVA444P, AV_PIX_FMT_YUVA420P,
        AV_PIX_FMT_NONE
    };
    AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
