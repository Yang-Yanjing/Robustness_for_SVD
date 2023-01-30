static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    PaletteUseContext *s = inlink->dst->priv;
    return ff_dualinput_filter_frame(&s->dinput, inlink, in);
}
