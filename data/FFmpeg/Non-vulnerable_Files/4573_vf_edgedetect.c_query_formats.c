static int query_formats(AVFilterContext *ctx)
{
    const EdgeDetectContext *edgedetect = ctx->priv;
    static const enum AVPixelFormat wires_pix_fmts[] = {AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE};
    static const enum AVPixelFormat colormix_pix_fmts[] = {AV_PIX_FMT_GBRP, AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE};
    AVFilterFormats *fmts_list;
    const enum AVPixelFormat *pix_fmts = NULL;
    if (edgedetect->mode == MODE_WIRES) {
        pix_fmts = wires_pix_fmts;
    } else if (edgedetect->mode == MODE_COLORMIX) {
        pix_fmts = colormix_pix_fmts;
    } else {
        av_assert0(0);
    }
    fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
