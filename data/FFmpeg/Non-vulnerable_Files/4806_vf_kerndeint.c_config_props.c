static int config_props(AVFilterLink *inlink)
{
    KerndeintContext *kerndeint = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int ret;
    kerndeint->is_packed_rgb = av_pix_fmt_desc_get(inlink->format)->flags & AV_PIX_FMT_FLAG_RGB;
    kerndeint->vsub = desc->log2_chroma_h;
    ret = av_image_alloc(kerndeint->tmp_data, kerndeint->tmp_linesize,
                         inlink->w, inlink->h, inlink->format, 16);
    if (ret < 0)
        return ret;
    memset(kerndeint->tmp_data[0], 0, ret);
    if ((ret = av_image_fill_linesizes(kerndeint->tmp_bwidth, inlink->format, inlink->w)) < 0)
        return ret;
    return 0;
}
