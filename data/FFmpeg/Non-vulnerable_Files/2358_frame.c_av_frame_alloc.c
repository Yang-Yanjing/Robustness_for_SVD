AVFrame *av_frame_alloc(void)
{
    AVFrame *frame = av_mallocz(sizeof(*frame));
    if (!frame)
        return NULL;
    frame->extended_data = NULL;
    get_frame_defaults(frame);
    return frame;
}
