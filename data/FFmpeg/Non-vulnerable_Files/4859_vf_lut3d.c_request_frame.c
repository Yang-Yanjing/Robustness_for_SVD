static int request_frame(AVFilterLink *outlink)
{
    LUT3DContext *s = outlink->src->priv;
    return ff_dualinput_request_frame(&s->dinput, outlink);
}
