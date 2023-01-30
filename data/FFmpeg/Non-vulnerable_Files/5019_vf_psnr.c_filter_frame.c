static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    PSNRContext *s = inlink->dst->priv;
    return ff_dualinput_filter_frame(&s->dinput, inlink, inpicref);
}
