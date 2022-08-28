static int config_input(AVFilterLink *inlink)
{
    IlContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int ret;
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    s->has_alpha = !!(desc->flags & AV_PIX_FMT_FLAG_ALPHA);
    if ((ret = av_image_fill_linesizes(s->linesize, inlink->format, inlink->w)) < 0)
        return ret;
    s->chroma_height = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    return 0;
}
