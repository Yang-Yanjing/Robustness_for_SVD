AVFILTER_DEFINE_CLASS(showpalette);
static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat in_fmts[]  = {AV_PIX_FMT_PAL8,  AV_PIX_FMT_NONE};
    static const enum AVPixelFormat out_fmts[] = {AV_PIX_FMT_RGB32, AV_PIX_FMT_NONE};
    AVFilterFormats *in  = ff_make_format_list(in_fmts);
    AVFilterFormats *out = ff_make_format_list(out_fmts);
    if (!in || !out) {
        av_freep(&in);
        av_freep(&out);
        return AVERROR(ENOMEM);
    }
    ff_formats_ref(in,  &ctx->inputs[0]->out_formats);
    ff_formats_ref(out, &ctx->outputs[0]->in_formats);
    return 0;
}
