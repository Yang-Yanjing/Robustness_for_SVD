static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    SSIMContext *s = inlink->dst->priv;
    return ff_dualinput_filter_frame(&s->dinput, inlink, buf);
}
