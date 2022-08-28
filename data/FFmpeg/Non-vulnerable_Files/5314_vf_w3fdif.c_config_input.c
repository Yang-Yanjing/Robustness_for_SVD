static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    W3FDIFContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    int ret, i;
    if ((ret = av_image_fill_linesizes(s->linesize, inlink->format, inlink->w)) < 0)
        return ret;
    s->planeheight[1] = s->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->planeheight[0] = s->planeheight[3] = inlink->h;
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    s->nb_threads = ctx->graph->nb_threads;
    s->work_line = av_calloc(s->nb_threads, sizeof(*s->work_line));
    if (!s->work_line)
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_threads; i++) {
        s->work_line[i] = av_calloc(s->linesize[0], sizeof(*s->work_line[0]));
        if (!s->work_line[i])
            return AVERROR(ENOMEM);
    }
    return 0;
}
