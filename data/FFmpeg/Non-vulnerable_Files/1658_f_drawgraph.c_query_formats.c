static int query_formats(AVFilterContext *ctx)
{
    AVFilterLink *outlink = ctx->outputs[0];
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_NONE
    };
    AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    ff_formats_ref(fmts_list, &outlink->in_formats);
    return 0;
}
