static int query_formats(AVFilterContext *ctx)
{
    const FadeContext *s = ctx->priv;
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_YUV444P,  AV_PIX_FMT_YUV422P,  AV_PIX_FMT_YUV420P,
        AV_PIX_FMT_YUV411P,  AV_PIX_FMT_YUV410P,
        AV_PIX_FMT_YUVJ444P, AV_PIX_FMT_YUVJ422P, AV_PIX_FMT_YUVJ420P,
        AV_PIX_FMT_YUV440P,  AV_PIX_FMT_YUVJ440P,
        AV_PIX_FMT_YUVA420P, AV_PIX_FMT_YUVA422P, AV_PIX_FMT_YUVA444P,
        AV_PIX_FMT_RGB24,    AV_PIX_FMT_BGR24,
        AV_PIX_FMT_ARGB,     AV_PIX_FMT_ABGR,
        AV_PIX_FMT_RGBA,     AV_PIX_FMT_BGRA,
        AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat pix_fmts_rgb[] = {
        AV_PIX_FMT_RGB24,    AV_PIX_FMT_BGR24,
        AV_PIX_FMT_ARGB,     AV_PIX_FMT_ABGR,
        AV_PIX_FMT_RGBA,     AV_PIX_FMT_BGRA,
        AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat pix_fmts_alpha[] = {
        AV_PIX_FMT_YUVA420P, AV_PIX_FMT_YUVA422P, AV_PIX_FMT_YUVA444P,
        AV_PIX_FMT_ARGB,     AV_PIX_FMT_ABGR,
        AV_PIX_FMT_RGBA,     AV_PIX_FMT_BGRA,
        AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat pix_fmts_rgba[] = {
        AV_PIX_FMT_ARGB,     AV_PIX_FMT_ABGR,
        AV_PIX_FMT_RGBA,     AV_PIX_FMT_BGRA,
        AV_PIX_FMT_NONE
    };
    AVFilterFormats *fmts_list;
    if (s->alpha) {
        if (s->black_fade)
            fmts_list = ff_make_format_list(pix_fmts_alpha);
        else
            fmts_list = ff_make_format_list(pix_fmts_rgba);
    } else {
        if (s->black_fade)
            fmts_list = ff_make_format_list(pix_fmts);
        else
            fmts_list = ff_make_format_list(pix_fmts_rgb);
    }
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
