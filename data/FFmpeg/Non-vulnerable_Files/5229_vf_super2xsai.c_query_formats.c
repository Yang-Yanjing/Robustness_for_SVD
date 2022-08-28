static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_RGBA, AV_PIX_FMT_BGRA, AV_PIX_FMT_ARGB, AV_PIX_FMT_ABGR,
        AV_PIX_FMT_RGB24, AV_PIX_FMT_BGR24,
        AV_PIX_FMT_RGB565BE, AV_PIX_FMT_BGR565BE, AV_PIX_FMT_RGB555BE, AV_PIX_FMT_BGR555BE,
        AV_PIX_FMT_RGB565LE, AV_PIX_FMT_BGR565LE, AV_PIX_FMT_RGB555LE, AV_PIX_FMT_BGR555LE,
        AV_PIX_FMT_NONE
    };
    AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
