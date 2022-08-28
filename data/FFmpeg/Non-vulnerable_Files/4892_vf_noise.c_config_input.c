static int config_input(AVFilterLink *inlink)
{
    NoiseContext *n = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int ret;
    n->nb_planes = av_pix_fmt_count_planes(inlink->format);
    if ((ret = av_image_fill_linesizes(n->bytewidth, inlink->format, inlink->w)) < 0)
        return ret;
    n->height[1] = n->height[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    n->height[0] = n->height[3] = inlink->h;
    return 0;
}
