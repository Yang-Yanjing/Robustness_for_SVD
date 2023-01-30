static int request_frame(AVFilterLink *outlink)
{
    OverlayContext *s = outlink->src->priv;
    return ff_dualinput_request_frame(&s->dinput, outlink);
}
