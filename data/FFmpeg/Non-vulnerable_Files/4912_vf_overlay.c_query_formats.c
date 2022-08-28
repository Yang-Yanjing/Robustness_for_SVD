static int query_formats(AVFilterContext *ctx)
{
    OverlayContext *s = ctx->priv;
    
    static const enum AVPixelFormat main_pix_fmts_yuv420[] = {
        AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUVA420P, AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat overlay_pix_fmts_yuv420[] = {
        AV_PIX_FMT_YUVA420P, AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat main_pix_fmts_yuv422[] = {
        AV_PIX_FMT_YUV422P, AV_PIX_FMT_YUVA422P, AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat overlay_pix_fmts_yuv422[] = {
        AV_PIX_FMT_YUVA422P, AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat main_pix_fmts_yuv444[] = {
        AV_PIX_FMT_YUV444P, AV_PIX_FMT_YUVA444P, AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat overlay_pix_fmts_yuv444[] = {
        AV_PIX_FMT_YUVA444P, AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat main_pix_fmts_rgb[] = {
        AV_PIX_FMT_ARGB,  AV_PIX_FMT_RGBA,
        AV_PIX_FMT_ABGR,  AV_PIX_FMT_BGRA,
        AV_PIX_FMT_RGB24, AV_PIX_FMT_BGR24,
        AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat overlay_pix_fmts_rgb[] = {
        AV_PIX_FMT_ARGB,  AV_PIX_FMT_RGBA,
        AV_PIX_FMT_ABGR,  AV_PIX_FMT_BGRA,
        AV_PIX_FMT_NONE
    };
    AVFilterFormats *main_formats;
    AVFilterFormats *overlay_formats;
    switch (s->format) {
    case OVERLAY_FORMAT_YUV420:
        main_formats    = ff_make_format_list(main_pix_fmts_yuv420);
        overlay_formats = ff_make_format_list(overlay_pix_fmts_yuv420);
        break;
    case OVERLAY_FORMAT_YUV422:
        main_formats    = ff_make_format_list(main_pix_fmts_yuv422);
        overlay_formats = ff_make_format_list(overlay_pix_fmts_yuv422);
        break;
    case OVERLAY_FORMAT_YUV444:
        main_formats    = ff_make_format_list(main_pix_fmts_yuv444);
        overlay_formats = ff_make_format_list(overlay_pix_fmts_yuv444);
        break;
    case OVERLAY_FORMAT_RGB:
        main_formats    = ff_make_format_list(main_pix_fmts_rgb);
        overlay_formats = ff_make_format_list(overlay_pix_fmts_rgb);
        break;
    default:
        av_assert0(0);
    }
    ff_formats_ref(main_formats,    &ctx->inputs [MAIN   ]->out_formats);
    ff_formats_ref(overlay_formats, &ctx->inputs [OVERLAY]->out_formats);
    ff_formats_ref(main_formats,    &ctx->outputs[MAIN   ]->in_formats );
    return 0;
}
