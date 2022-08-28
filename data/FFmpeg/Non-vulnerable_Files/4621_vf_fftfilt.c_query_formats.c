static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat pixel_fmts_fftfilt[] = {
        AV_PIX_FMT_GRAY8,
        AV_PIX_FMT_YUV444P,
        AV_PIX_FMT_NONE
    };
    AVFilterFormats *fmts_list = ff_make_format_list(pixel_fmts_fftfilt);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
