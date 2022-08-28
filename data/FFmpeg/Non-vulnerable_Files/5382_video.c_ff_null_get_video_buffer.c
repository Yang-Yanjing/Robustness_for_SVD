AVFrame *ff_null_get_video_buffer(AVFilterLink *link, int w, int h)
{
    return ff_get_video_buffer(link->dst->outputs[0], w, h);
}
