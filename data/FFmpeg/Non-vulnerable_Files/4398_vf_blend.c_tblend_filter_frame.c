static int tblend_filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    BlendContext *s = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    if (s->prev_frame) {
        AVFrame *out = blend_frame(inlink->dst, frame, s->prev_frame);
        av_frame_free(&s->prev_frame);
        s->prev_frame = frame;
        return ff_filter_frame(outlink, out);
    }
    s->prev_frame = frame;
    return 0;
}
