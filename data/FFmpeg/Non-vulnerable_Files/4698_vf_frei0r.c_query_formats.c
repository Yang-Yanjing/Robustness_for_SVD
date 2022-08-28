static int query_formats(AVFilterContext *ctx)
{
    Frei0rContext *s = ctx->priv;
    AVFilterFormats *formats = NULL;
    if        (s->plugin_info.color_model == F0R_COLOR_MODEL_BGRA8888) {
        ff_add_format(&formats, AV_PIX_FMT_BGRA);
    } else if (s->plugin_info.color_model == F0R_COLOR_MODEL_RGBA8888) {
        ff_add_format(&formats, AV_PIX_FMT_RGBA);
    } else {                                   
        static const enum AVPixelFormat pix_fmts[] = {
            AV_PIX_FMT_BGRA, AV_PIX_FMT_ARGB, AV_PIX_FMT_ABGR, AV_PIX_FMT_ARGB, AV_PIX_FMT_NONE
        };
        formats = ff_make_format_list(pix_fmts);
    }
    if (!formats)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, formats);
}
