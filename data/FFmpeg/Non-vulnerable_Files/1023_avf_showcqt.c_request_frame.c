static int request_frame(AVFilterLink *outlink)
{
    ShowCQTContext *s = outlink->src->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    int ret;
    s->req_fullfilled = 0;
    do {
        ret = ff_request_frame(inlink);
    } while (!s->req_fullfilled && ret >= 0);
    if (ret == AVERROR_EOF && s->outpicref)
        filter_frame(inlink, NULL);
    return ret;
}
