static int request_frame(AVFilterLink *outlink)
{
    return ff_request_frame(outlink->src->inputs[0]);
}
