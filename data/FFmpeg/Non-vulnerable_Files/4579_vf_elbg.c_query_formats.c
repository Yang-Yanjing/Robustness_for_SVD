static int query_formats(AVFilterContext *ctx)
{
    ELBGContext *elbg = ctx->priv;
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_ARGB, AV_PIX_FMT_RGBA, AV_PIX_FMT_ABGR, AV_PIX_FMT_BGRA,
        AV_PIX_FMT_RGB24, AV_PIX_FMT_BGR24,
        AV_PIX_FMT_NONE
    };
    if (!elbg->pal8) {
        AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
        if (!fmts_list)
            return AVERROR(ENOMEM);
        return ff_set_common_formats(ctx, fmts_list);
    } else {
        static const enum AVPixelFormat pal8_fmt[] = {
            AV_PIX_FMT_PAL8,
            AV_PIX_FMT_NONE
        };
        ff_formats_ref(ff_make_format_list(pix_fmts), &ctx->inputs[0]->out_formats);
        ff_formats_ref(ff_make_format_list(pal8_fmt), &ctx->outputs[0]->in_formats);
    }
    return 0;
}
