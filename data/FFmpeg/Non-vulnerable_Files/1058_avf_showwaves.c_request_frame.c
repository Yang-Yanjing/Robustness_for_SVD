static int request_frame(AVFilterLink *outlink)
{
    ShowWavesContext *showwaves = outlink->src->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    int ret;
    showwaves->req_fullfilled = 0;
    do {
        ret = ff_request_frame(inlink);
    } while (!showwaves->req_fullfilled && ret >= 0);
    if (ret == AVERROR_EOF && showwaves->outpicref) {
        if (showwaves->single_pic)
            push_single_pic(outlink);
        else
            push_frame(outlink);
    }
    return ret;
}
