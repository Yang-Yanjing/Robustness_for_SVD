static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    NContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int ret;
    if ((ret = av_image_fill_linesizes(s->planewidth, inlink->format, inlink->w)) < 0)
        return ret;
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    s->buffer = av_malloc(3 * (s->planewidth[0] + 32));
    if (!s->buffer)
        return AVERROR(ENOMEM);
    if (!strcmp(ctx->filter->name, "erosion"))
        s->filter = erosion;
    else if (!strcmp(ctx->filter->name, "dilation"))
        s->filter = dilation;
    else if (!strcmp(ctx->filter->name, "deflate"))
        s->filter = deflate;
    else if (!strcmp(ctx->filter->name, "inflate"))
        s->filter = inflate;
    return 0;
}
