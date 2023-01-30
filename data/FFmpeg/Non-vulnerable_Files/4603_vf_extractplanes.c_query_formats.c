AVFILTER_DEFINE_CLASS(extractplanes);
static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat in_pixfmts[] = {
        AV_PIX_FMT_YUV410P,
        AV_PIX_FMT_YUV411P,
        AV_PIX_FMT_YUV440P,
        AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUVA420P,
        AV_PIX_FMT_YUV422P, AV_PIX_FMT_YUVA422P,
        AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_YUVJ422P,
        AV_PIX_FMT_YUVJ440P, AV_PIX_FMT_YUVJ444P,
        AV_PIX_FMT_YUVJ411P,
        AV_PIX_FMT_YUV444P, AV_PIX_FMT_YUVA444P,
        AV_PIX_FMT_YUV420P16LE, AV_PIX_FMT_YUVA420P16LE,
        AV_PIX_FMT_YUV420P16BE, AV_PIX_FMT_YUVA420P16BE,
        AV_PIX_FMT_YUV422P16LE, AV_PIX_FMT_YUVA422P16LE,
        AV_PIX_FMT_YUV422P16BE, AV_PIX_FMT_YUVA422P16BE,
        AV_PIX_FMT_YUV444P16LE, AV_PIX_FMT_YUVA444P16LE,
        AV_PIX_FMT_YUV444P16BE, AV_PIX_FMT_YUVA444P16BE,
        AV_PIX_FMT_GRAY8, AV_PIX_FMT_GRAY8A,
        AV_PIX_FMT_YA16LE, AV_PIX_FMT_YA16BE,
        AV_PIX_FMT_GRAY16LE, AV_PIX_FMT_GRAY16BE,
        AV_PIX_FMT_RGB24, AV_PIX_FMT_BGR24,
        AV_PIX_FMT_RGBA, AV_PIX_FMT_BGRA,
        AV_PIX_FMT_ARGB, AV_PIX_FMT_ABGR,
        AV_PIX_FMT_RGB48LE, AV_PIX_FMT_BGR48LE,
        AV_PIX_FMT_RGB48BE, AV_PIX_FMT_BGR48BE,
        AV_PIX_FMT_RGBA64LE, AV_PIX_FMT_BGRA64LE,
        AV_PIX_FMT_RGBA64BE, AV_PIX_FMT_BGRA64BE,
        AV_PIX_FMT_GBRP, AV_PIX_FMT_GBRAP,
        AV_PIX_FMT_GBRP16LE, AV_PIX_FMT_GBRP16BE,
        AV_PIX_FMT_GBRAP16LE, AV_PIX_FMT_GBRAP16BE,
        AV_PIX_FMT_NONE,
    };
    static const enum AVPixelFormat out8_pixfmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };
    static const enum AVPixelFormat out16le_pixfmts[] = { AV_PIX_FMT_GRAY16LE, AV_PIX_FMT_NONE };
    static const enum AVPixelFormat out16be_pixfmts[] = { AV_PIX_FMT_GRAY16BE, AV_PIX_FMT_NONE };
    const enum AVPixelFormat *out_pixfmts;
    const AVPixFmtDescriptor *desc;
    AVFilterFormats *avff;
    int i, depth = 0, be = 0;
    if (!ctx->inputs[0]->in_formats ||
        !ctx->inputs[0]->in_formats->nb_formats) {
        return AVERROR(EAGAIN);
    }
    if (!ctx->inputs[0]->out_formats)
        ff_formats_ref(ff_make_format_list(in_pixfmts), &ctx->inputs[0]->out_formats);
    avff = ctx->inputs[0]->in_formats;
    desc = av_pix_fmt_desc_get(avff->formats[0]);
    depth = desc->comp[0].depth_minus1;
    be = desc->flags & AV_PIX_FMT_FLAG_BE;
    for (i = 1; i < avff->nb_formats; i++) {
        desc = av_pix_fmt_desc_get(avff->formats[i]);
        if (depth != desc->comp[0].depth_minus1 ||
            be    != (desc->flags & AV_PIX_FMT_FLAG_BE)) {
            return AVERROR(EAGAIN);
        }
    }
    if (depth == 7)
        out_pixfmts = out8_pixfmts;
    else if (be)
        out_pixfmts = out16be_pixfmts;
    else
        out_pixfmts = out16le_pixfmts;
    for (i = 0; i < ctx->nb_outputs; i++)
        ff_formats_ref(ff_make_format_list(out_pixfmts), &ctx->outputs[i]->in_formats);
    return 0;
}
