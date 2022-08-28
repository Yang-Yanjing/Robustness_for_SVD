static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *pix_fmts = NULL;
    int fmt;
    for (fmt = 0; av_pix_fmt_desc_get(fmt); fmt++) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(fmt);
        if (!(desc->flags & AV_PIX_FMT_FLAG_PAL ||
              desc->flags & AV_PIX_FMT_FLAG_HWACCEL ||
              desc->flags & AV_PIX_FMT_FLAG_BITSTREAM))
            ff_add_format(&pix_fmts, fmt);
    }
    return ff_set_common_formats(ctx, pix_fmts);
}
