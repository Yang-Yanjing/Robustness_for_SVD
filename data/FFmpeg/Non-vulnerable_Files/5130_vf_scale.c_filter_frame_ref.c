static int filter_frame_ref(AVFilterLink *link, AVFrame *in)
{
    AVFilterLink *outlink = link->dst->outputs[1];
    return ff_filter_frame(outlink, in);
}
