static int query_formats(AVFilterContext *ctx)
{
    SelectContext *select = ctx->priv;
    if (!select->do_scene_detect) {
        return ff_default_query_formats(ctx);
    } else {
        int ret;
        static const enum AVPixelFormat pix_fmts[] = {
            AV_PIX_FMT_RGB24, AV_PIX_FMT_BGR24,
            AV_PIX_FMT_NONE
        };
        AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
        if (!fmts_list)
            return AVERROR(ENOMEM);
        ret = ff_set_common_formats(ctx, fmts_list);
        if (ret < 0)
            return ret;
    }
    return 0;
}
