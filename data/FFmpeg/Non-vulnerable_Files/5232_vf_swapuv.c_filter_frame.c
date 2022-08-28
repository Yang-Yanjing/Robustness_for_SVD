static int filter_frame(AVFilterLink *link, AVFrame *inpicref)
{
    do_swap(inpicref);
    return ff_filter_frame(link->dst->outputs[0], inpicref);
}
