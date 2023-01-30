static int query_formats(AVFilterContext *ctx)
{
    AVFilterFormats *formats = NULL;
    int fmt;
    for (fmt = 0; av_pix_fmt_desc_get(fmt); fmt++) {
        const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(fmt);
        if (desc->flags & AV_PIX_FMT_FLAG_PLANAR && !((desc->comp[0].depth_minus1 + 1) & 7))
            ff_add_format(&formats, fmt);
    }
    return ff_set_common_formats(ctx, formats);
}
