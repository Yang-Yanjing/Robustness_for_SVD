static int query_formats(AVFilterContext *ctx)
{
    HistogramContext *h = ctx->priv;
    const enum AVPixelFormat *pix_fmts;
    AVFilterFormats *fmts_list;
    switch (h->mode) {
    case MODE_WAVEFORM:
        pix_fmts = waveform_pix_fmts;
        break;
    case MODE_LEVELS:
    {
        AVFilterFormats *avff;
        const AVPixFmtDescriptor *desc;
        const enum AVPixelFormat *out_pix_fmts;
        int rgb, i, bits;
        if (!ctx->inputs[0]->in_formats ||
            !ctx->inputs[0]->in_formats->nb_formats) {
            return AVERROR(EAGAIN);
        }
        if (!ctx->inputs[0]->out_formats)
            ff_formats_ref(ff_make_format_list(levels_in_pix_fmts), &ctx->inputs[0]->out_formats);
        avff = ctx->inputs[0]->in_formats;
        desc = av_pix_fmt_desc_get(avff->formats[0]);
        rgb = desc->flags & AV_PIX_FMT_FLAG_RGB;
        bits = desc->comp[0].depth_minus1;
        for (i = 1; i < avff->nb_formats; i++) {
            desc = av_pix_fmt_desc_get(avff->formats[i]);
            if ((rgb != (desc->flags & AV_PIX_FMT_FLAG_RGB)) ||
                (bits != desc->comp[0].depth_minus1))
                return AVERROR(EAGAIN);
        }
        if (rgb && bits == 7)
            out_pix_fmts = levels_out_rgb8_pix_fmts;
        else if (rgb && bits == 8)
            out_pix_fmts = levels_out_rgb9_pix_fmts;
        else if (rgb && bits == 9)
            out_pix_fmts = levels_out_rgb10_pix_fmts;
        else if (bits == 7)
            out_pix_fmts = levels_out_yuv8_pix_fmts;
        else if (bits == 8)
            out_pix_fmts = levels_out_yuv9_pix_fmts;
        else 
            out_pix_fmts = levels_out_yuv10_pix_fmts;
        ff_formats_ref(ff_make_format_list(out_pix_fmts), &ctx->outputs[0]->in_formats);
        return 0;
    }
        break;
    case MODE_COLOR:
    case MODE_COLOR2:
        pix_fmts = color_pix_fmts;
        break;
    default:
        av_assert0(0);
    }
    fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}
