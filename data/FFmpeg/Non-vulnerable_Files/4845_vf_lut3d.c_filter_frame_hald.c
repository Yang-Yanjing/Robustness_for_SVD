static int filter_frame_hald(AVFilterLink *inlink, AVFrame *inpicref)
{
    LUT3DContext *s = inlink->dst->priv;
    return ff_dualinput_filter_frame(&s->dinput, inlink, inpicref);
}
