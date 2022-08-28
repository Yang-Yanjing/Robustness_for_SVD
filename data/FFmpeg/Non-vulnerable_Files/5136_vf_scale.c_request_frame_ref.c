static int request_frame_ref(AVFilterLink *outlink)
{
    return ff_request_frame(outlink->src->inputs[1]);
}
