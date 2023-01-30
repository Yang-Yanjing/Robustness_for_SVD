static int query_formats(AVFilterContext *ctx)
{
    VectorscopeContext *s = ctx->priv;
    const enum AVPixelFormat *out_pix_fmts;
    const AVPixFmtDescriptor *desc;
    AVFilterFormats *avff;
    int rgb, i;
    if (!ctx->inputs[0]->in_formats ||
        !ctx->inputs[0]->in_formats->nb_formats) {
        return AVERROR(EAGAIN);
    }
    if (!ctx->inputs[0]->out_formats) {
        const enum AVPixelFormat *in_pix_fmts;
        if ((s->x == 1 && s->y == 2) || (s->x == 2 && s->y == 1))
            in_pix_fmts = in2_pix_fmts;
        else
            in_pix_fmts = in1_pix_fmts;
        ff_formats_ref(ff_make_format_list(in_pix_fmts), &ctx->inputs[0]->out_formats);
    }
    avff = ctx->inputs[0]->in_formats;
    desc = av_pix_fmt_desc_get(avff->formats[0]);
    rgb = desc->flags & AV_PIX_FMT_FLAG_RGB;
    for (i = 1; i < avff->nb_formats; i++) {
        desc = av_pix_fmt_desc_get(avff->formats[i]);
        if (rgb != (desc->flags & AV_PIX_FMT_FLAG_RGB))
            return AVERROR(EAGAIN);
    }
    if (rgb)
        out_pix_fmts = out_rgb_pix_fmts;
    else
        out_pix_fmts = out_yuv_pix_fmts;
    ff_formats_ref(ff_make_format_list(out_pix_fmts), &ctx->outputs[0]->in_formats);
    return 0;
}
