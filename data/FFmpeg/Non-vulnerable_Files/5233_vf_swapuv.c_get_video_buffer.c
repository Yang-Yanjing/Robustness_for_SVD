static AVFrame *get_video_buffer(AVFilterLink *link, int w, int h)
{
    AVFrame *picref = ff_default_get_video_buffer(link, w, h);
    do_swap(picref);
    return picref;
}
