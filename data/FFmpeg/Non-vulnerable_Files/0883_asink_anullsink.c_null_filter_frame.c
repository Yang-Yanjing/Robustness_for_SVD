static int null_filter_frame(AVFilterLink *link, AVFrame *frame)
{
    av_frame_free(&frame);
    return 0;
}
