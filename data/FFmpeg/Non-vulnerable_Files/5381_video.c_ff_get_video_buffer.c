AVFrame *ff_get_video_buffer(AVFilterLink *link, int w, int h)
{
    AVFrame *ret = NULL;
    FF_TPRINTF_START(NULL, get_video_buffer); ff_tlog_link(NULL, link, 0);
    if (link->dstpad->get_video_buffer)
        ret = link->dstpad->get_video_buffer(link, w, h);
    if (!ret)
        ret = ff_default_get_video_buffer(link, w, h);
    return ret;
}
