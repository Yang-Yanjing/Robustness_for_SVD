static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    BlendContext *s = inlink->dst->priv;
    return ff_dualinput_filter_frame(&s->dinput, inlink, buf);
}
