static int request_frame(AVFilterLink *outlink)
{
    DecimateContext *decimate = outlink->src->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    int ret;
    do {
        ret = ff_request_frame(inlink);
    } while (decimate->drop_count > 0 && ret >= 0);
    return ret;
}
