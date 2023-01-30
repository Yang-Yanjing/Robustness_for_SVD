static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat main_fmts[] = {
        AV_PIX_FMT_YUVA444P, AV_PIX_FMT_YUVA422P, AV_PIX_FMT_YUVA420P,
        AV_PIX_FMT_RGBA, AV_PIX_FMT_BGRA, AV_PIX_FMT_ARGB, AV_PIX_FMT_ABGR,
        AV_PIX_FMT_NONE
    };
    static const enum AVPixelFormat alpha_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };
    AVFilterFormats *main_formats = ff_make_format_list(main_fmts);
    AVFilterFormats *alpha_formats = ff_make_format_list(alpha_fmts);
    ff_formats_ref(main_formats, &ctx->inputs[0]->out_formats);
    ff_formats_ref(alpha_formats, &ctx->inputs[1]->out_formats);
    ff_formats_ref(main_formats, &ctx->outputs[0]->in_formats);
    return 0;
}
