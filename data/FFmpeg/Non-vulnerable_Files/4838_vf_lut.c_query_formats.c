static int query_formats(AVFilterContext *ctx)
{
    LutContext *s = ctx->priv;
    const enum AVPixelFormat *pix_fmts = s->is_rgb ? rgb_pix_fmts :
                                                     s->is_yuv ? yuv_pix_fmts :
                                                                 all_pix_fmts;
    AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
