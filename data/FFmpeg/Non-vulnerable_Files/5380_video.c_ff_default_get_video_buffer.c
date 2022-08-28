AVFrame *ff_default_get_video_buffer(AVFilterLink *link, int w, int h)
{
    AVFrame *frame = av_frame_alloc();
    int ret;
    if (!frame)
        return NULL;
    frame->width  = w;
    frame->height = h;
    frame->format = link->format;
    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0)
        av_frame_free(&frame);
    return frame;
}
