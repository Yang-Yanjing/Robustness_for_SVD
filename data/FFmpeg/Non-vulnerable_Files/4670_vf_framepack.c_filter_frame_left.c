static int filter_frame_left(AVFilterLink *inlink, AVFrame *frame)
{
    FramepackContext *s = inlink->dst->priv;
    s->input_views[LEFT] = frame;
    return 0;
}
