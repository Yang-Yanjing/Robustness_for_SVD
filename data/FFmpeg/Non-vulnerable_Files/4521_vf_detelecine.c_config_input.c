static int config_input(AVFilterLink *inlink)
{
    DetelecineContext *s = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int ret;
    s->temp = ff_get_video_buffer(inlink, inlink->w, inlink->h);
    if (!s->temp)
        return AVERROR(ENOMEM);
    s->frame = ff_get_video_buffer(inlink, inlink->w, inlink->h);
    if (!s->frame)
        return AVERROR(ENOMEM);
    if ((ret = av_image_fill_linesizes(s->stride, inlink->format, inlink->w)) < 0)
        return ret;
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    return 0;
}
