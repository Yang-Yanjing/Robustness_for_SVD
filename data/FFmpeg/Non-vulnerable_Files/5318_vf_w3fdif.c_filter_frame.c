static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    W3FDIFContext *s = ctx->priv;
    int ret;
    av_frame_free(&s->prev);
    s->prev = s->cur;
    s->cur  = s->next;
    s->next = frame;
    if (!s->cur) {
        s->cur = av_frame_clone(s->next);
        if (!s->cur)
            return AVERROR(ENOMEM);
    }
    if ((s->deint && !s->cur->interlaced_frame) || ctx->is_disabled) {
        AVFrame *out = av_frame_clone(s->cur);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_free(&s->prev);
        if (out->pts != AV_NOPTS_VALUE)
            out->pts *= 2;
        return ff_filter_frame(ctx->outputs[0], out);
    }
    if (!s->prev)
        return 0;
    ret = filter(ctx, 0);
    if (ret < 0)
        return ret;
    return filter(ctx, 1);
}
