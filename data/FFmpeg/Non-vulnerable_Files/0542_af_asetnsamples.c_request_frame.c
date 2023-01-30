static int request_frame(AVFilterLink *outlink)
{
    AVFilterLink *inlink = outlink->src->inputs[0];
    int ret;
    ret = ff_request_frame(inlink);
    if (ret == AVERROR_EOF) {
        ret = push_samples(outlink);
        return ret < 0 ? ret : ret > 0 ? 0 : AVERROR_EOF;
    }
    return ret;
}
