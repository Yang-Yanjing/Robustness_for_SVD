static int filter_frame_right(AVFilterLink *inlink, AVFrame *frame)
{
    FramepackContext *s = inlink->dst->priv;
    s->input_views[RIGHT] = frame;
    return 0;
}
