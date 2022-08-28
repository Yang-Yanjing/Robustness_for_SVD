static int default_filter_frame(AVFilterLink *link, AVFrame *frame)
{
    return ff_filter_frame(link->dst->outputs[0], frame);
}
