static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    AVFilterContext *ctx = inlink->dst;
    SeparateFieldsContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    int ret;
    inpicref->height = outlink->h;
    inpicref->interlaced_frame = 0;
    if (!s->second) {
        goto clone;
    } else {
        AVFrame *second = s->second;
        extract_field(second, s->nb_planes, second->top_field_first);
        if (second->pts != AV_NOPTS_VALUE &&
            inpicref->pts != AV_NOPTS_VALUE)
            second->pts += inpicref->pts;
        else
            second->pts = AV_NOPTS_VALUE;
        ret = ff_filter_frame(outlink, second);
        if (ret < 0)
            return ret;
clone:
        s->second = av_frame_clone(inpicref);
        if (!s->second)
            return AVERROR(ENOMEM);
    }
    extract_field(inpicref, s->nb_planes, !inpicref->top_field_first);
    if (inpicref->pts != AV_NOPTS_VALUE)
        inpicref->pts *= 2;
    return ff_filter_frame(outlink, inpicref);
}
