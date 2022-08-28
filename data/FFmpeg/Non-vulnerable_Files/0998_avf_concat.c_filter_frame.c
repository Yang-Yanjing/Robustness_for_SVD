static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    return process_frame(inlink, buf);
}
